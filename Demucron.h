#pragma once

#include <vector>
#include <algorithm>

#include "GraphData.h"
#include "IGraph.h"
#include "AdjancencyVectorGraph.h"

IGraphData::Levels demucron(IGraph const& graph)
{
  size_t nTotalNodes = graph.getTotalNodes();

  std::vector<uint32_t> columnsSumm;
  columnsSumm.resize(nTotalNodes, 0);

  NodesVector neighbors;
  neighbors.reserve(graph.getMaxNodeLevel());

  for (IGraph::NodeId nodeId = 0; nodeId < nTotalNodes; ++nodeId) {
    graph.getNeighbors(nodeId, neighbors);
    for (IGraph::NodeId const& neighborId : neighbors)
      ++columnsSumm[neighborId];
  }

  IGraphData::Levels levels;
  while (true) {
    NodesVector nodesOnLevel;
    for(IGraph::NodeId nNodeId = 0; nNodeId < nTotalNodes; ++nNodeId) {
      if (!columnsSumm[nNodeId]) {
        nodesOnLevel.push_back(nNodeId);
        columnsSumm[nNodeId] = uint32_t(-1);  // this cell won't be used anymore
      }
    }

    for (IGraph::NodeId excludedNodeId : nodesOnLevel) {
      graph.getNeighbors(excludedNodeId, neighbors);
      for (IGraph::NodeId const& neighborId : neighbors)
        --columnsSumm[neighborId];
    }

    if (nodesOnLevel.empty())
      break;
    levels.push_back(std::move(nodesOnLevel));
  };

  return levels;
}
