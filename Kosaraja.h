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
  // Построим инверсный граф
  AdjancencyVectorGraph inversedGraph;
  inversedGraph.buildFromOther(graph, true);

  // В nodes будут хранится все узлы графа в порядке их посещения
  // при обходе графа поиском вглубину (DFS)
  NodesVector nodes;
  nodes.reserve(nTotalNodes);
  // blackList - массив, который позволяет за O(1) определить, был
  // ли узел nodeId уже обработан (blackList[nodeId] == true) или
  // ещё нет (blackList[nodeId] == false)
  std::vector<bool> blackList(nTotalNodes, false);
  for (IGraph::NodeId nodeId = 0; nodeId < nTotalNodes; ++nodeId)
    if (!blackList[nodeId])
      inversedGraph.DFS(nodeId, nodes, blackList);

  // Выполняем DFS для узлов из nodes в обратном порядке
  // Все посещённые узлы в рамках одного вызова DFS() будут
  // формировать один кластер (компонент сильной связности).
  std::reverse(nodes.begin(), nodes.end());
  blackList.assign(blackList.size(), false);
  for (IGraph::NodeId nodeId : nodes)
  {
    if (!blackList[nodeId]) {
      // Вызываем DFS() только для тех узлов из nodes, которые
      // не были посещены при предыдущих вызовах DFS, т.к. ещё
      // не принадлежат ни одному кластеру
      NodesVector cluster;
      graph.DFS(nodeId, cluster, blackList);
      clusters.push_back(std::move(cluster));
    }
  }
}
