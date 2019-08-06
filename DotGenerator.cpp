#include "DotGenerator.h"
#include "Utils.h"
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

std::string generateNodesInfo(IGraph const& graph, IGraphData const& data,
                              std::string const& sLinePrefix)
{
  std::stringstream ss;
  size_t nTotalNodes = graph.getTotalNodes();
  for(size_t nNodeId = 0; nNodeId < nTotalNodes; ++nNodeId) {
    std::string const& sNodeName = data.getNodeName(nNodeId);
    std::string const& sNodeColor = colorAsDotString(data.getNodeColorARGB(nNodeId));

    ss << sLinePrefix << nNodeId << " ["
          "label = \"" << sNodeName << "\", " <<
          "color = \"" << sNodeColor << "\", " <<
          "style = filled" <<
          "];\n";
  }
  return ss.str();
}

std::string generateLevels(IGraphData const& data, std::string const& sLinePrefix)
{
  std::stringstream ss;

  IGraphData::Levels const& levels = data.getLevels();

  for (NodesVector const& level : levels) {
    ss << sLinePrefix << "{ rank = same; ";
    for (IGraph::NodeId nodeId : level)
      ss << nodeId << "; ";
    ss << "}\n";
  }

  return ss.str();
}

std::string generateGraphviz(IGraph const& graph, IGraphData const& data)
{
  std::stringstream ss;
  ss << "digraph G {\n" << generateNodesInfo(graph, data, "  ") << "\n\n" <<
        generateLevels(data, "  ") << "\n\n" <<
        generateEdges(graph, "  ") << "}";
  return ss.str();
}
