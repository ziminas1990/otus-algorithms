#include "DotGenerator.h"
#include <sstream>

std::string enumerateNodes(std::vector<IGraph::NodeId> const& nodes)
{
  std::stringstream ss;
  for(size_t i = 0; i < nodes.size(); ++i) {
    ss << nodes[i];
    if (i < nodes.size() - 1)
      ss << ", ";
  }
  return ss.str();
}

std::string generateEdges(IGraph const& graph, std::string const& sLinePrefix)
{
  std::stringstream ss;

  size_t nTotalNodes = graph.getTotalNodes();
  for(size_t nNodeId = 0; nNodeId < nTotalNodes; ++nNodeId) {
    ss << sLinePrefix << nNodeId;
    std::vector<IGraph::NodeId> neighbors;
    graph.getNeighbors(nNodeId, neighbors);
    if (!neighbors.empty())
      ss << " -> " << enumerateNodes(neighbors);
    ss << ";\n";
  }
  return ss.str();
}

std::string generateNodesInfo(IGraph const& graph, INodeStorage const& data,
                              std::string const& sLinePrefix)
{
  std::stringstream ss;
  size_t nTotalNodes = graph.getTotalNodes();
  for(size_t nNodeId = 0; nNodeId < nTotalNodes; ++nNodeId) {
    ss << sLinePrefix << nNodeId << " [label = \"";
    ss << data.getNodeName(nNodeId);
    ss << "\"];\n";
  }
  return ss.str();
}

std::string generateDot(IGraph const& graph, INodeStorage const& data)
{
  std::stringstream ss;
  ss << "digraph G {\n" << generateNodesInfo(graph, data, "  ") << "\n\n" <<
        generateEdges(graph, "  ") << "}";
  return ss.str();
}
