#include <iostream>
#include <stdint.h>
#include <set>
#include <map>
#include <assert.h>
#include <sstream>

#include "Utils.h"
#include "AdjancencyVectorGraph.h"
#include "DotGenerator.h"

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

  std::string getNodeName(IGraph::NodeId const& nNodeId) const override
  {
    return (nNodeId < m_nodes.size()) ? m_nodes[nNodeId].asString() : std::string();
  }

private:
  std::vector<Node>           m_nodes;
  std::map<T, IGraph::NodeId> m_valueToNodeId;
};

template<typename T>
void buildGraph(IGraph& graph, GraphData<T>& graphData)
{
  std::string sLine;
  while (std::getline(std::cin, sLine)) {
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
  GraphData<std::string> graphData;
  AdjancencyVectorGraph  graphTopology;
  buildGraph(graphTopology, graphData);

  std::cout << "\n" << generateDot(graphTopology, graphData) << std::endl;
  return 0;
}
