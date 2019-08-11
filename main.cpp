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
#include "RandomGraphGenerator.h"
#include "Deikstra.h"


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

void pathOnRandomGraph()
{
  std::srand(time(nullptr));

  AdjancencyMatrixGraph graph;
  randomGraphGenerator(graph, 32);

  NodesVector path;
  for (size_t i = 0; i < 1000; ++i) {
    NodesVector newPath;
    deikstra(graph, rand() % graph.getTotalNodes(), rand() % graph.getTotalNodes(),
             newPath);
    if (newPath.size() > path.size())
      path = std::move(newPath);
  }

  GraphData<int> graphData;
  for (size_t i = 1; i < path.size(); ++i)
    graphData.setEdgeColor(path[i - 1], path[i], 0x00FF0000);
  std::cout << "\n" << generateGraphviz(graph, graphData) << std::endl;
}

int main(int argc, char* argv[])
{ 
  std::srand(time(nullptr));

  if (argc < 2) {
    pathOnRandomGraph();
    return 0;
  }

  if (argc < 4)
    return 0;

  std::ifstream file;
  file.open(argv[1]);
  if (!file.good()) {
    std::cerr << "FAILED to open file " << argv[1] << std::endl;
    return 1;
  }

  AdjancencyMatrixGraph  graph;
  buildGraph(file, graph);

  NodeId nStart  = atol(argv[2]);
  NodeId nFinish = atol(argv[3]);

  if (nStart >= graph.getTotalNodes() || nFinish >= graph.getTotalNodes())
    return 0;

  NodesVector path;
  deikstra(graph, nStart, nFinish, path);

  if (argc == 5 && std::string(argv[4]) == "dot") {
    GraphData<int> graphData;
    for (size_t i = 1; i < path.size(); ++i)
      graphData.setEdgeColor(path[i - 1], path[i], 0x00FF0000);

    std::cout << "\n" << generateGraphviz(graph, graphData) << std::endl;
  } else {
    // printing path
    for (NodeId node : path)
      std::cout << node << (node == nFinish ? "" : " -> ");
  }
  return 0;
}
