#pragma once

#include "IGraph.h"
#include "Utils.h"

#include <math.h>

struct Node2D
{
  void randomPlacement(double nMinR, double nMaxR) {
    double r = nMinR + ((rand() % 1000) / 1000.0) * (nMaxR - nMinR);
    assert(r >= nMinR && r < nMaxR);
    double a = (rand() % 1000) * M_PI / 1000.0;
    x = r * cos(a);
    y = r * sin(a);
  }

  double distance(Node2D const& other) const {
    double dx = x - other.x;
    double dy = y - other.y;
    return sqrt(dx * dx + dy * dy);
  }

  double x = 0;
  double y = 0;
};

void attachToNeighbors(
    IWeightedGraph& graph, std::vector<Node2D> allNodes, NodeId nodeId,
    size_t nTotalEdges)
{
  assert(nodeId < allNodes.size());
  std::vector<std::pair<double, NodeId>> distances;
  distances.reserve(allNodes.size());

  for (NodeId otherNodeId = 0; otherNodeId < allNodes.size(); ++otherNodeId) {
    double distance = allNodes[otherNodeId].distance(allNodes[nodeId]);
    if (distance < 1)
      continue;
    distances.push_back(std::make_pair(distance, otherNodeId));
  }

  assert(!distances.empty());
  std::sort(distances.begin(), distances.end());

  size_t nQuarter = distances.size() / 4;
  if (nQuarter)
    std::random_shuffle(distances.begin(), distances.begin() + nQuarter);

  for (size_t i = 0; i < std::min(distances.size(), nTotalEdges); ++i)
    graph.addEdge(nodeId, distances[i].second, distances[i].first);
}

void randomGraphGenerator(IWeightedGraph& graph, size_t nTotalNodes)
{
  std::vector<Node2D> nodes;

  // one node in (0, 0)
  graph.addNode();
  nodes.push_back(Node2D());

  double nMaxR = 5;
  double nMinR = 1;

  for(size_t i = 1; i < nTotalNodes; ++i) {
    NodeId newNode = graph.addNode();
    assert(newNode == nodes.size());

    Node2D nodeOnSurface;
    nodeOnSurface.randomPlacement(nMinR, nMaxR);
    nodes.push_back(nodeOnSurface);

    size_t nEdges = 1;
    if (rand() % 2 == 0)
      ++nEdges;
    if (rand() % 4 == 0)
      ++nEdges;
    if (rand() % 9 == 0)
      ++nEdges;

    attachToNeighbors(graph, nodes, newNode, 1 + rand() % 2);

    if (nTotalNodes % i == 0) {
      nMaxR += 0.4;
      nMinR += 0.3;
    }
  }
}
