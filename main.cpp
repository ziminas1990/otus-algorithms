#include <iostream>
#include <fstream>
#include <stdint.h>
#include <set>
#include <map>
#include <assert.h>
#include <sstream>

#include "Utils.h"
#include "AdjancencyVectorGraph.h"
#include "DotGenerator.h"
#include "Kosaraja.h"

template<typename T>
class GraphData : public INodeStorage
{
  struct Node
  {
    std::string asString() const {
      std::stringstream ss;
      ss << value;
      return ss.str();
    }

    T value;
    IGraph::NodeId nNodeId = IGraph::InvalidNodeId();
  };

public:

  // overrides from INodeStorage
  std::string getNodeName(IGraph::NodeId const& nNodeId) const override
  {
    return (nNodeId < m_nodes.size()) ? m_nodes[nNodeId].asString() : std::string();
  }

  uint32_t getNodeColorARGB(IGraph::NodeId const& nNodeId) const override
  {
    static const uint32_t nWhite = 0x00FFFFFF;

    uint32_t nClusterId;
    {
      auto I = m_nodeIdToClusterId.find(nNodeId);
      if (I == m_nodeIdToClusterId.end())
        return nWhite;
      nClusterId = I->second;
    }

    auto I = m_clustersColors.find(nClusterId);
    if (I == m_clustersColors.end())
      return nWhite;
    return I->second;
  }


  IGraph::NodeId getOrCreateNode(T const& nodeValue, IGraph& graph)
  {
    auto I = m_valueToNodeId.find(nodeValue);
    if (I != m_valueToNodeId.end())
      return I->second;
    Node newNode;
    newNode.value   = nodeValue;
    newNode.nNodeId = graph.addNode();
    assert(newNode.nNodeId == m_nodes.size());
    m_valueToNodeId[nodeValue] = newNode.nNodeId;
    m_nodes.push_back(std::move(newNode));
    return m_nodes.back().nNodeId;
  }

  void registerClusters(Clusters const& clusters)
  {
    m_nodeIdToClusterId.clear();
    m_clustersColors.clear();
    size_t nClusterId = 0;
    for (NodesVector const& cluster : clusters) {
      m_clustersColors[nClusterId] = createdRandomColor();
      for (IGraph::NodeId const& nNodeId : cluster)
        m_nodeIdToClusterId[nNodeId] = nClusterId;
      ++nClusterId;
    }
  }

private:
  std::vector<Node>           m_nodes;
  std::map<T, IGraph::NodeId> m_valueToNodeId;

  // index - nodeId, value - cluster of node
  std::map<IGraph::NodeId, uint32_t> m_nodeIdToClusterId;
  // key - clusterId, value - color
  std::map<uint32_t, uint32_t> m_clustersColors;
};

template<typename T>
void buildGraph(std::istream& input, IGraph& graph, GraphData<T>& graphData)
{
  std::string sLine;
  while (std::getline(input, sLine)) {
    std::stringstream ss(sLine);
    T nodeValue;
    ss >> nodeValue;

    IGraph::NodeId nodeId = graphData.getOrCreateNode(nodeValue, graph);
    while(!ss.eof()) {
      ss >> nodeValue;
      IGraph::NodeId neighbord = graphData.getOrCreateNode(nodeValue, graph);
      graph.addEdge(nodeId, neighbord);
    }
  }
}

int main(int argc, char* argv[])
{
  std::srand(time(nullptr));

  std::istream* input = &std::cin;
  std::ifstream file;
  if (argc == 2) {
    file.open(argv[1]);
    if (!file.good()) {
      std::cerr << "FAILED to open file " << argv[1] << std::endl;
      return 1;
    }
    input = &file;
  }

  GraphData<std::string> graphData;
  AdjancencyVectorGraph  graph;
  buildGraph(*input, graph, graphData);

  Clusters clusters;
  kosaraja(graph, clusters);

  graphData.registerClusters(clusters);

  std::cout << "\n" << generateDot(graph, graphData) << std::endl;
  return 0;
}
