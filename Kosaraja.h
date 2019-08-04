#pragma once

#include <vector>
#include <algorithm>

#include "IGraph.h"
#include "AdjancencyVectorGraph.h"

using NodesVector = std::vector<IGraph::NodeId>;
using Clusters    = std::vector<NodesVector>;

void kosaraja(IGraph const& graph, Clusters& clusters)
{
  size_t nTotalNodes = graph.getTotalNodes();
  AdjancencyVectorGraph inversedGraph;
  inversedGraph.buildFromOther(graph, true);

  NodesVector nodes;
  nodes.reserve(nTotalNodes);

  std::vector<bool> blackList(nTotalNodes, false);
  for (IGraph::NodeId nodeId = 0; nodeId < nTotalNodes; ++nodeId)
    if (!blackList[nodeId])
      inversedGraph.DFS(nodeId, nodes, blackList);

  std::reverse(nodes.begin(), nodes.end());

  blackList.assign(blackList.size(), false);
  for (IGraph::NodeId nodeId : nodes)
  {
    if (!blackList[nodeId]) {
      NodesVector cluster;
      graph.DFS(nodeId, cluster, blackList);
      clusters.push_back(std::move(cluster));
    }
  }
}
