#pragma once

#include "IGraph.h"
#include <vector>
#include <assert.h>

class AdjancencyMatrixGraph : public IWeightedGraph
{
public:
  NodeId addNode() override;
  size_t getTotalNodes() const override { return  m_matrix.size(); }

  // Both functions has O(N) comlicity:
  size_t getNeighborsCount(NodeId nNodeId) const override;
  size_t getNeighbors(NodeId nNodeId, std::vector<NodeId> &out,
                      bool lAppend) const override;


  void     addEdge(NodeId nOneNodeId, NodeId nOtherNodeId, uint32_t nWeight) override;
  uint32_t getEdge(NodeId nOneNodeId, NodeId nOtherNodeId) const override;

private:
  std::vector<std::vector<uint32_t>> m_matrix;
};


inline NodeId AdjancencyMatrixGraph::addNode()
{
  size_t nTotalNodes = m_matrix.size() + 1;
  m_matrix.push_back(std::vector<uint32_t>());

  std::vector<uint32_t>& nNewNodeEdges = m_matrix.back();
  // 0 - means that edge doesn't exist
  nNewNodeEdges.resize(nTotalNodes, 0);

  for (size_t i = 0; i < nTotalNodes - 1; ++i) {
    std::vector<uint32_t>& edges = m_matrix[i];
    edges.push_back(0);
  }

  return nTotalNodes - 1;
}


inline size_t AdjancencyMatrixGraph::getNeighborsCount(NodeId nNodeId) const
{
  if (nNodeId >= m_matrix.size())
    return 0;
  size_t nTotal = 0;
  std::vector<uint32_t> const& edges = m_matrix[nNodeId];
  for (uint32_t nWeight : edges)
    if (nWeight)
      ++nTotal;
  return nTotal;
}


inline size_t AdjancencyMatrixGraph::getNeighbors(
    NodeId nNodeId, std::vector<NodeId>& out, bool lAppend) const
{
  if (nNodeId >= m_matrix.size())
    return 0;
  if (!lAppend)
    out.clear();

  size_t nTotal = 0;
  std::vector<uint32_t> const& edges = m_matrix[nNodeId];
  for (NodeId nNodeId = 0; nNodeId < edges.size(); ++nNodeId)
    if (edges[nNodeId])
      out.push_back(nNodeId);
  return nTotal;
}

inline
void AdjancencyMatrixGraph::addEdge(NodeId nOneNodeId,
                                    NodeId nOtherNodeId,
                                    uint32_t nWeight)
{
  assert(nOneNodeId < m_matrix.size());
  assert(nOtherNodeId < m_matrix.size());

  // yes, duplicating memory :(
  m_matrix[nOneNodeId][nOtherNodeId] = nWeight;
  m_matrix[nOtherNodeId][nOneNodeId] = nWeight;
}

inline
uint32_t AdjancencyMatrixGraph::getEdge(NodeId nOneNodeId, NodeId nOtherNodeId) const
{
  assert(nOneNodeId < m_matrix.size());
  assert(nOtherNodeId < m_matrix.size());
  return m_matrix[nOneNodeId][nOtherNodeId];
}
