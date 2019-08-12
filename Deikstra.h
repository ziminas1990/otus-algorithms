#pragma once

#include <stack>
#include <map>
#include <stdint.h>
#include <assert.h>
#include <algorithm>

#include "IGraph.h"

// Контекст узла хранит длину nShortestPath маршрута, который выходит из
// некоторой начальной точки и приходит в данную точку от её соседа nPreviousNode
struct NodeContext {
  NodeContext() = default;
  NodeContext(NodeId nodeId, uint32_t nShortestPathLength,
              NodeId nPreviosNode)
    : nNodeId(nodeId), nShortestPath(nShortestPathLength),
      nPreviousNode(nPreviosNode)
  {}

  bool isValid() const { return nNodeId != IGraph::InvalidNodeId(); }

  NodeId   nNodeId        = IGraph::InvalidNodeId();
  uint32_t nShortestPath  = 0;
  NodeId   nPreviousNode  = IGraph::InvalidNodeId();
};

// Вспомогательная функция
// Данная функция просматривает контекст узлов, пути до которых известны, но
// которые ещё не были посещены, находит узел с наиболее коротким путём до
// него, возвращает его и удаляет из контекста
static
NodeContext visitNodeWithShortestPath(std::map<NodeId, NodeContext>& context)
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

  NodeContext result = itShortest->second;
  context.erase(itShortest);
  return result;
}

static bool deikstra(IWeightedGraph const& graph, NodeId nStart, NodeId nFinish,
                     NodesVector& path)
{
  // В контексте context записаны узлы, расстояние до которых уже известно, но
  // которые ещё НЕ были обработаны (посещены)
  std::map<NodeId, NodeContext> context;
  // Вектор, в котором отмечаются посещённые узлы. Индекс - номер узла,
  // значение - факт посещения. Позволяет за O(1) определить, был ли узел
  // посещён ранее
  std::vector<NodeContext> visitedNodes(graph.getTotalNodes());

  // изначально в контексте только стартовый узел. С него и начнётся обработка
  context[nStart] = NodeContext(nStart, 0, IGraph::InvalidNodeId());

  while (!context.empty()) {
    // Находим непосещённый узел с минимальной длиной пути до него, и берём
    // его в обработку (помечаем как посещённый)
    NodeContext currentNode    = visitNodeWithShortestPath(context);
    NodeId      nCurrentNodeId = currentNode.nNodeId;
    visitedNodes[nCurrentNodeId] = currentNode;
    if (nCurrentNodeId == nFinish)
      break; // Выбранный узел является конечной точкой - маршрут найден.

    NodesVector neighbors;
    graph.getNeighbors(nCurrentNodeId, neighbors);
    if (neighbors.empty())
      continue;

    // Проходим по всем соседям выбранного узла
    for (NodeId neighbor : neighbors) {
      if (visitedNodes[neighbor].isValid())
        continue; // Узлы, которые ранее были обработаны, игнорируются

      // вычисляем найденный путь до соседа (путь до текущего узла + длина ребра)
      NodeContext& neighborContext = context[neighbor];
      uint32_t nEdgeLength     = graph.getEdge(nCurrentNodeId, neighbor);
      uint32_t nPathToNeighbor = currentNode.nShortestPath + nEdgeLength;

      if (!neighborContext.isValid() ||
          nPathToNeighbor < neighborContext.nShortestPath) {
        // если узел ранее не был достижим, либо если новый найденный путь до него
        // оказался короче найденного ранее, то запишем в контекст длину найденного пути
        neighborContext = NodeContext(neighbor, nPathToNeighbor, nCurrentNodeId);
      }
    }
  }

  path.reserve(10); // Эвристическая оптимизация ¯\_(ツ)_/¯

  // "Раскручиваем" маршрут, двигаемся от конечной точки к начальной,
  // записывая порядок следования узлов
  NodeId nCurrentNodeId = nFinish;
  while(nCurrentNodeId != IGraph::InvalidNodeId()) {
    NodeContext const& nodeContext = visitedNodes[nCurrentNodeId];
    path.push_back(nodeContext.nNodeId);
    nCurrentNodeId = nodeContext.nPreviousNode;
  }
  // Развернём порядок следования узлов
  std::reverse(path.begin(), path.end());
  return true;
}
