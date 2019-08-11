#pragma once

#include <stack>
#include <map>
#include <stdint.h>
#include <assert.h>
#include <algorithm>

#include "IGraph.h"

struct NodeContext {
  NodeContext() = default;
  NodeContext(NodeId nodeId, uint32_t nShortestPathLength,
              NodeId nPreviosNode)
    : nNodeId(nodeId), nShortestPath(nShortestPathLength),
      nPreviousNode(nPreviosNode)
  {}

  bool isValid() const { return nNodeId != IGraph::InvalidNodeId(); }

  NodeId   nNodeId        = IGraph::InvalidNodeId();
  uint32_t nShortestPath  = 0;
  NodeId   nPreviousNode  = IGraph::InvalidNodeId();
};

// looks in list an element, that has minimal nShortestPath value, mark it as visited
// and returns it
NodeContext visitNodeWithShortestPath(std::map<NodeId, NodeContext>& context)
{
  if (context.empty())
    return NodeContext();

  auto itCurrent = context.begin();

  uint32_t nShortestPath = itCurrent->second.nShortestPath;
  auto itShortest        = itCurrent;

  while (itCurrent != context.end()) {
    NodeContext& context = itCurrent->second;
    if (context.nShortestPath < nShortestPath) {
      nShortestPath = context.nShortestPath;
      itShortest    = itCurrent;
    }
    ++itCurrent;
  }

  NodeContext result = itShortest->second;
  context.erase(itShortest);
  return result;
}

static bool deikstra(IWeightedGraph const& graph, NodeId nStart, NodeId nFinish,
                     NodesVector& path)
{
  std::map<NodeId, NodeContext> context;
  // index - NodeId; if value is True, that means that node has been visited already
  std::vector<NodeContext> visitedNodes(graph.getTotalNodes());

  context[nStart] = NodeContext(nStart, 0, IGraph::InvalidNodeId());

  while (!context.empty()) {
    // looking for node, that has shortest path:
    NodeContext currentNode    = visitNodeWithShortestPath(context);
    NodeId      nCurrentNodeId = currentNode.nNodeId;
    visitedNodes[nCurrentNodeId] = currentNode;
    if (nCurrentNodeId == nFinish)
      break;

    NodesVector neighbors;
    graph.getNeighbors(nCurrentNodeId, neighbors);
    if (neighbors.empty())
      continue;

    for (NodeId neighbor : neighbors) {
      if (visitedNodes[neighbor].isValid())
        continue; // ignoring nodes, that has been handled already

      NodeContext& neighborContext = context[neighbor];
      uint32_t nEdgeLength     = graph.getEdge(nCurrentNodeId, neighbor);
      uint32_t nPathToNeighbor = currentNode.nShortestPath + nEdgeLength;

      if (!neighborContext.isValid() ||
          nPathToNeighbor < neighborContext.nShortestPath) {
        // node is visited at first OR
        // path to neighbor from current node is shorter, path, that was discovered before
        neighborContext = NodeContext(neighbor, nPathToNeighbor, nCurrentNodeId);
      }
    }
  }

  path.reserve(10); // ¯\_(ツ)_/¯

  NodeId nCurrentNodeId = nFinish;
  while(nCurrentNodeId != IGraph::InvalidNodeId()) {
    NodeContext const& nodeContext = visitedNodes[nCurrentNodeId];
    path.push_back(nodeContext.nNodeId);
    nCurrentNodeId = nodeContext.nPreviousNode;
  }
  std::reverse(path.begin(), path.end());
  return true;
}
