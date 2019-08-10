#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <set>
#include <stack>

using NodeId = uint32_t;

// Note: complecity of each function depends on implementation of graph
class IGraph
{
public:
  using NodePair = std::pair<NodeId, NodeId>;

  static NodeId InvalidNodeId() { return NodeId(-1); }

  virtual ~IGraph() = default;

  virtual NodeId addNode() = 0;
  virtual size_t getTotalNodes() const = 0;

  virtual size_t getNeighborsCount(NodeId nNodeId) const = 0;
  virtual size_t getNeighbors(NodeId nNodeId, std::vector<NodeId>& out,
                              bool lAppend = false) const = 0;
};


struct Edge
{
  Edge()
    : nOneNodeId(IGraph::InvalidNodeId()), nOtherNodeId(IGraph::InvalidNodeId()),
      nWeight(0)
  {}

  Edge(NodeId nOneNodeId, NodeId nOtherNodeId, uint32_t nWeight)
    : nOneNodeId(nOneNodeId), nOtherNodeId(nOtherNodeId), nWeight(nWeight)
  {}

  bool isValid() const {
    return nOneNodeId   != IGraph::InvalidNodeId()
        && nOtherNodeId != IGraph::InvalidNodeId();
  }

  bool operator<(Edge const& other) const { return nWeight < other.nWeight; }

  NodeId   nOneNodeId   = IGraph::InvalidNodeId();
  NodeId   nOtherNodeId = IGraph::InvalidNodeId();
  uint32_t nWeight      = 0;
};


class IWeightedGraph : public IGraph
{
public:

  virtual void     addEdge(NodeId nOneNodeId, NodeId nOtherNodeId, uint32_t nWeight) = 0;
  // if returns 0, that means that edge doesn't exist
  virtual uint32_t getEdge(NodeId nOneNodeId, NodeId nOtherNodeId) const = 0;

  virtual void exportAllEdges(std::vector<Edge>& out) const;
};


using NodesVector = std::vector<NodeId>;
using NodesSet    = std::set<NodeId>;


inline void IWeightedGraph::exportAllEdges(std::vector<Edge>& out) const
{
  size_t nTotalNodes  = getTotalNodes();
  NodeId nOneNodeId   = 0;
  NodeId nOtherNodeId = 0;

  for (nOneNodeId = 0; nOneNodeId < nTotalNodes; ++nOneNodeId) {
    for (nOtherNodeId = nOneNodeId + 1; nOtherNodeId < nTotalNodes; ++nOtherNodeId) {
      uint32_t nWeight = getEdge(nOneNodeId, nOtherNodeId);
      if (nWeight)
        out.emplace_back(nOneNodeId, nOtherNodeId, nWeight);
    }
  }
}
