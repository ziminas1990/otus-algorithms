#include <iostream>
#include <fstream>
#include <stdint.h>
#include <set>
#include <assert.h>
#include <sstream>

#include "Utils.h"
#include "GraphData.h"
#include "AdjancencyMatrixGraph.h"
#include "DotGenerator.h"
#include "Kruskal.h"


void buildGraph(std::istream& input, IWeightedGraph& graph)
{
  std::string sLine;

  NodeId   nOneNodeId   = 0;
  NodeId   nOtherNodeId = 0;
  uint32_t nWeight      = 0;
  size_t   nTotalNodes  = 0;

  while (std::getline(input, sLine)) {
    std::stringstream ss(sLine);
    if (nOneNodeId == nTotalNodes) {
      graph.addNode();
      ++nTotalNodes;
    }

    nOtherNodeId = 0;
    while(!ss.eof()) {
      if (nOtherNodeId == nTotalNodes) {
        graph.addNode();
        ++nTotalNodes;
      }

      ss >> nWeight;
      graph.addEdge(nOneNodeId, nOtherNodeId, nWeight);
      ++nOtherNodeId;
    }
    ++nOneNodeId;
  }
}

int main(int argc, char* argv[])
{
  if (argc < 2)
    return 0;
  std::srand(time(nullptr));

  std::ifstream file;
  file.open(argv[1]);
  if (!file.good()) {
    std::cerr << "FAILED to open file " << argv[1] << std::endl;
    return 1;
  }

  AdjancencyMatrixGraph  graph;
  buildGraph(file, graph);

  std::vector<Edge> minSpanningTreeEdges;
  kruskal(graph, minSpanningTreeEdges);

  if (argc == 3 && std::string(argv[2]) == "dot") {
    GraphData<int> graphData;
    for (Edge const& edge : minSpanningTreeEdges)
      graphData.setEdgeColor(edge.nOneNodeId, edge.nOtherNodeId, 0x00FF0000);
    std::cout << "\n" << generateGraphviz(graph, graphData) << std::endl;
  } else {
    // printing edges of spanning tree graph:
    for (Edge const& edge : minSpanningTreeEdges)
      std::cout << edge.nOneNodeId << " -- " << edge.nOtherNodeId << std::endl;
  }
  return 0;
}
