#include <iostream>
#include <fstream>
#include <stdint.h>
#include <set>
#include <assert.h>
#include <sstream>

#include "Utils.h"
#include "GraphData.h"
#include "AdjancencyVectorGraph.h"
#include "DotGenerator.h"
#include "Demucron.h"


void buildGraph(std::istream& input, IGraph& graph, GraphData<int>& graphData)
{
  std::string sLine;

  int nCurrentNode = 0;
  int nChildNode   = 0;

  while (std::getline(input, sLine)) {
    std::stringstream ss(sLine);
    IGraph::NodeId nodeId = graphData.getOrCreateNode(nCurrentNode, graph);
    while(!ss.eof()) {
      ss >> nChildNode;
      IGraph::NodeId neighbord = graphData.getOrCreateNode(nChildNode, graph);
      graph.addEdge(nodeId, neighbord);
    }
    ++nCurrentNode;
  }
}

int main(int argc, char* argv[])
{
  std::srand(time(nullptr));

  std::istream* input = &std::cin;
  std::ifstream file;
  if (argc >= 2) {
    file.open(argv[1]);
    if (!file.good()) {
      std::cerr << "FAILED to open file " << argv[1] << std::endl;
      return 1;
    }
    input = &file;
  }

  GraphData<int> graphData;
  AdjancencyVectorGraph  graph;
  buildGraph(*input, graph, graphData);

  IGraphData::Levels levels = demucron(graph);

  if (argc == 3 && std::string(argv[2]) == "dot") {
    // printing result as graph
    graphData.setLevels(std::move(levels));
    std::cout << "\n" << generateGraphviz(graph, graphData) << std::endl;

  } else {
    // printing result as matrix
    for (size_t nLevelId = 0; nLevelId < levels.size(); ++nLevelId) {
      std::cout << nLevelId << "\t";
      for (IGraph::NodeId nodeId : levels[nLevelId])
        std::cout << " " << nodeId;
      std::cout << std::endl;
    }
  }
  return 0;
}
