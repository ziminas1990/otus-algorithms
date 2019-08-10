#pragma once

#include <stack>
#include <map>
#include <stdint.h>
#include <assert.h>

#include "IGraph.h"

struct NodeContext {
  NodeContext() = default;
  NodeContext(NodeId nodeId, uint32_t nShortestPathLength,
              NodeId nPreviosNode)
    : nNodeId(nNodeId), lVisited(false), nShortestPath(nShortestPathLength),
      nPreviousNode(nPreviousNode)
  {}

  bool isValid() const { return nNodeId != IGraph::InvalidNodeId(); }

  NodeId   nNodeId        = IGraph::InvalidNodeId();
  bool     lVisited       = false;
  uint32_t nShortestPath  = 0;
  NodeId   nPreviousNode  = IGraph::InvalidNodeId();
};

// looks in list an element, that has minimal nShortestPath value, mark it as visited
// and returns it
NodeContext const& visitNodeWithShortestPath(std::map<NodeId, NodeContext>& context)
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

  itShortest->second.lVisited = true;
  return itShortest->second;
}

static bool deikstra(IWeightedGraph const& graph, NodeId nStart, NodeId nFinish,
                     NodesVector& path)
{
  std::map<NodeId, NodeContext> context;
  std::stack<NodeId>            visitedNodes;

  context[nStart] = NodeContext(nStart, 0, IGraph::InvalidNodeId());

  while (!context.empty()) {
    // looking for node, that has shortest path:
    NodeContext const& currentNode    = visitNodeWithShortestPath(context);
    NodeId             nCurrentNodeId = currentNode.nNodeId;
    visitedNodes.push(nCurrentNodeId);
    if (nCurrentNodeId == nFinish)
      break;

    NodesVector neighbors;
    graph.getNeighbors(nCurrentNodeId, neighbors);
    if (neighbors.empty())
      continue;

    for (NodeId neighbor : neighbors) {
      NodeContext& neighborContext = context[neighbor];
      if (neighborContext.isValid() && neighborContext.lVisited)
        // node has been visited already
        continue;

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

  if (visitedNodes.top() != nFinish)
    return false; // path not found

  path.reserve(visitedNodes.size());
  while (!visitedNodes.empty()) {
    path.push_back(visitedNodes.top());
    visitedNodes.pop();
  }
  return true;
}
