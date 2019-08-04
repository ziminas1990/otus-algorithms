#pragma once

#include "IGraph.h"
#include <vector>

class AdjancencyVectorGraph : public IGraph
{
public:
  NodeId addNode() override {
    m_graph.emplace_back();
    return m_graph.size() - 1;
  }

  size_t getTotalNodes() const override { return m_graph.size(); }

  void addEdge(NodeId nFrom, NodeId nTo) override { m_graph.at(nFrom).push_back(nTo); }

  size_t getNeighborsCount(NodeId nNodeId) const override {
    return m_graph.at(nNodeId).size();
  }

  size_t getNeighbors(NodeId nNodeId, std::vector<NodeId>& out) const override {
    Neighbors const& neighbors = m_graph.at(nNodeId);
    out.insert(out.end(), neighbors.begin(), neighbors.end());
    return neighbors.size();
  }

private:
  using Neighbors = std::vector<NodeId>;

  // index is NodeId, value is NodeId's neighbors
  std::vector<Neighbors> m_graph;
};
