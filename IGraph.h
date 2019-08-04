#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <stack>

// Note: complecity of each function depends on implementation of graph
class IGraph
{
public:
  using NodeId = uint32_t;
  static NodeId InvalidNodeId() { return NodeId(-1); }

  virtual ~IGraph() = default;

  virtual NodeId addNode() = 0;
  virtual size_t getTotalNodes() const = 0;

  virtual void addEdge(NodeId nFrom, NodeId nTo) = 0;

  virtual size_t getNeighborsCount(NodeId nNodeId) const = 0;
  virtual size_t getNeighbors(NodeId nNodeId, std::vector<NodeId>& out) const = 0;

  virtual void buildFromOther(IGraph const& other, bool lInverseEdges = false);

  // Default implementation probably has very heigh complicity!
  virtual size_t getMaxNodeLevel() const;

  size_t DFS(NodeId nFirstNode, std::vector<NodeId>& visitedNodes,
             std::vector<bool>& blackList) const;
};

inline void IGraph::buildFromOther(IGraph const& other, bool lInverseEdges)
{
  size_t nTotalNodes = other.getTotalNodes();
  for(size_t i = 0; i < nTotalNodes; ++i)
    addNode();

  std::vector<NodeId> neighbors;
  neighbors.reserve(other.getMaxNodeLevel());

  for(uint32_t nNodeId = 0; nNodeId < nTotalNodes; ++nNodeId) {
    neighbors.clear();
    other.getNeighbors(nNodeId, neighbors);
    for (NodeId nNeighborId : neighbors) {
      if (!lInverseEdges) {
        addEdge(nNodeId, nNeighborId);
      } else {
        addEdge(nNeighborId, nNodeId);
      }
    }
  }
}

inline size_t IGraph::getMaxNodeLevel() const {
  size_t nTotalNodes = getTotalNodes();
  size_t nMaxLevel   = 0;
  for (NodeId nNodeId = 0; nNodeId < nTotalNodes; ++nNodeId) {
    nMaxLevel = std::max(getNeighborsCount(nNodeId), nMaxLevel);
  }
  return nMaxLevel;
}

inline size_t IGraph::DFS(IGraph::NodeId nRootNodeId,
                          std::vector<IGraph::NodeId> &visitedNodes,
                          std::vector<bool>& blackList) const
{
  if (blackList[nRootNodeId])
    return 0;
  blackList[nRootNodeId] = true;

  std::vector<NodeId> neighbors;
  neighbors.reserve(10);
  neighbors.clear();
  getNeighbors(nRootNodeId, neighbors);

  for (NodeId nChild : neighbors)
    if (!blackList[nChild])
      DFS(nChild, visitedNodes, blackList);

  visitedNodes.push_back(nRootNodeId);
}
