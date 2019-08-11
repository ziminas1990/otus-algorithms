#include "DotGenerator.h"
#include "Utils.h"
#include <sstream>

std::string generateEdges(IWeightedGraph const& graph, IGraphData const& data,
                          std::string const& sLinePrefix)
{
  std::stringstream ss;

  NodeId nOneNode    = 0;
  NodeId nOtherNode  = 0;
  size_t nTotalNodes = graph.getTotalNodes();

  for (nOneNode = 0; nOneNode < nTotalNodes; ++nOneNode) {
    for (nOtherNode = nOneNode + 1; nOtherNode < nTotalNodes; ++nOtherNode) {
      double nWeight = graph.getEdge(nOneNode, nOtherNode);
      if (nWeight) {
        ss << sLinePrefix << nOneNode << " -> " << nOtherNode <<
              " [dir = none, label = \"~" << int(nWeight * 10) << "\", len = " << nWeight <<
              ", color = \"" <<
              colorAsDotString(data.getEdgeColor(nOneNode, nOtherNode)) << "\"]\n";
      }
    }
  }
  return ss.str();
}

std::string generateGraphviz(IWeightedGraph const& graph, IGraphData const& data)
{
  std::stringstream ss;
  ss << "digraph G {\n" << generateEdges(graph, data, "  ") << "}";
  return ss.str();
}
