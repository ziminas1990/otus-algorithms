#pragma once

#include <set>
#include <vector>
#include <stdint.h>
#include "IGraph.h"
#include <assert.h>

#include <algorithm>

class NodesClusters
{
public:
  NodesClusters() : m_clusters(1), m_nTotalNodes(0) {}

  size_t getTotalNodes() const { return m_nTotalNodes; }

  // returns clusterId (> 0), that include node nodeId; otherwise returns 0
  size_t find(NodeId nodeId) const
  {
    for (size_t nClusterId = 1; nClusterId < m_clusters.size(); ++nClusterId) {
      NodesSet const& cluster = m_clusters[nClusterId];
      if (cluster.find(nodeId) != cluster.end())
        return nClusterId;
    }
    return 0;
  }

  size_t newCluster(NodeId nOneNodeId, NodeId nOtherNodeId)
  {
    assert(find(nOneNodeId) == 0);
    assert(find(nOtherNodeId) == 0);
    NodesSet cluster;
    cluster.insert(nOneNodeId);
    cluster.insert(nOtherNodeId);
    m_clusters.push_back(std::move(cluster));
    m_nTotalNodes += 2;
    return m_clusters.size() - 1;
  }

  void addNodeToCluster(size_t nClusterId, NodeId nNodeId)
  {
    assert(nClusterId && nClusterId < m_clusters.size());

    bool lInserted = m_clusters[nClusterId].insert(nNodeId).second;
    assert(lInserted);
    ++m_nTotalNodes;
  }

  // Merge one cluster to another and return id of merged cluster
  size_t mergeClusters(size_t nOneClusterId, size_t nAnotherClusterId)
  {
    assert(nOneClusterId != nAnotherClusterId);
    NodesSet& to   = m_clusters[std::min(nOneClusterId, nAnotherClusterId)];
    NodesSet& from = m_clusters[std::min(nOneClusterId, nAnotherClusterId)];
    to.insert(from.begin(), from.end());
    from.clear();
    return std::min(nOneClusterId, nAnotherClusterId);
  }

private:
  // index - clusterId, value - nodes in cluster
  std::vector<NodesSet> m_clusters;

  size_t m_nTotalNodes;
};

static void kruskal(std::vector<Edge> allEdges, size_t nTotalNodes,
                    std::vector<Edge>& minSpanTreeEdges)
{
  std::sort(allEdges.begin(), allEdges.end());

  NodesClusters clusters;
  for(Edge const& edge : allEdges) {
    size_t nOneClusterId   = clusters.find(edge.nOneNodeId);
    size_t nOtherClusterId = clusters.find(edge.nOtherNodeId);
    if (nOneClusterId == nOtherClusterId) {
      if (nOneClusterId == 0) {
        clusters.newCluster(edge.nOneNodeId, edge.nOtherNodeId);
      } else {
        // this edge will create cycle and should be skipped
        continue;
      }
    } else if (!nOneClusterId) {
      clusters.addNodeToCluster(nOtherClusterId, edge.nOneNodeId);
    } else if (!nOtherClusterId) {
      clusters.addNodeToCluster(nOneClusterId, edge.nOtherNodeId);
    } else {
      // nodes belong to different clusters - merging clusters
      clusters.mergeClusters(nOneClusterId, nOtherClusterId);
    }

    minSpanTreeEdges.push_back(edge);
    if (clusters.getTotalNodes() == nTotalNodes)
      return;
  }
}

// just a wrapper
static void kruskal(IWeightedGraph const& graph, std::vector<Edge>& minSpanTreeEdges)
{
  std::vector<Edge> edges;
  graph.exportAllEdges(edges);
  kruskal(std::move(edges), graph.getTotalNodes(), minSpanTreeEdges);
}
