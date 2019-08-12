#pragma once

#include <vector>
#include <algorithm>

#include "GraphData.h"
#include "IGraph.h"
#include "AdjancencyVectorGraph.h"

IGraphData::Levels demucron(IGraph const& graph)
{
  size_t nTotalNodes = graph.getTotalNodes();

  // columnsSumm - массив, где индекс - это номер узла графа,
  // а значение - количество рёбер, входящих в данный узел (уровень)
  // Т.е. i-я ячейка - это сумма значений в i-м столбце матрицы
  // инцендентности
  std::vector<uint32_t> columnsSumm;
  columnsSumm.resize(nTotalNodes, 0);

  NodesVector neighbors;
  neighbors.reserve(graph.getMaxNodeLevel());

  // Посчитаем исходное состояние массива columnsSumm, т.е. уровень
  // всех узлов графа
  for (IGraph::NodeId nodeId = 0; nodeId < nTotalNodes; ++nodeId) {
    graph.getNeighbors(nodeId, neighbors);
    for (IGraph::NodeId const& neighborId : neighbors)
      ++columnsSumm[neighborId];
  }

  // Основной цикл алгоритма: на каждой итерации находим узлы без "входящих"
  // ребёр, записываем их в текущий уровень и вычитаем их рёбра из массива
  // columnsSumm (что аналогично их удалению из графа)
  IGraphData::Levels levels;
  while (true) {
    // Все узлы без входящих ребёр записываем в текущий уровень (nodesOnLevel)
    NodesVector nodesOnLevel;
    for(IGraph::NodeId nNodeId = 0; nNodeId < nTotalNodes; ++nNodeId) {
      if (!columnsSumm[nNodeId]) {
        nodesOnLevel.push_back(nNodeId);
        columnsSumm[nNodeId] = uint32_t(-1);  // this cell won't be used anymore
      }
    }

    // Корректируем массив columnsSumm, вычитая из него рёбра узлов, которые
    // были помещены в nodeOnLevel
    for (IGraph::NodeId excludedNodeId : nodesOnLevel) {
      graph.getNeighbors(excludedNodeId, neighbors);
      for (IGraph::NodeId const& neighborId : neighbors)
        --columnsSumm[neighborId];
    }

    if (nodesOnLevel.empty()) {
      // если попали сюда, значит либо алгоритм завершился, либо наткнулся на цикл
      break;
    }
    levels.push_back(std::move(nodesOnLevel));
  };

  return levels;
}
