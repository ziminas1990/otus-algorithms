#include <iostream>
#include <stdint.h>
#include <set>
#include <map>
#include <sstream>

#include "Utils.h"
#include "AdjancencyVectorGraph.h"
#include "DotGenerator.h"

template<typename T>
struct Node
{
  Node() : nNodeId(IGraph::InvalidNodeId()) {}
  //Node(T value) : value(std::move(value)), nNodeId(IGraph::InvalidNodeId()) {}
  Node(IGraph::NodeId nNodeId, T value) : value(std::move(value)), nNodeId(nNodeId) {}

  bool isValid() const { return nNodeId != IGraph::InvalidNodeId(); }

  T              value;
  IGraph::NodeId nNodeId = IGraph::InvalidNodeId();
};


template<typename T>
using NodesMap = std::map<T, Node<T>>;

template<typename T>
Node<T>& getOrCreateNode(T const& value, IGraph& graph, NodesMap<T>& nodes)
{
  auto itNode = nodes.find(value);
  if (itNode != nodes.end())
    return itNode->second;
  Node<T> newNode;
  newNode.value   = value;
  newNode.nNodeId = graph.addNode();
  return nodes.insert(std::make_pair(value, std::move(newNode))).first->second;
}

template<typename T>
void buildGraph(IGraph& graph, std::map<IGraph::NodeId, Node<T>>& nodesInfo)
{
  NodesMap<T> nodes;
  std::string sLine;
  while (std::getline(std::cin, sLine)) {
    std::stringstream ss(sLine);
    T nodeValue;
    ss >> nodeValue;

    Node<T>& node = getOrCreateNode(nodeValue, graph, nodes);
    nodesInfo[node.nNodeId] = node;

    while(!ss.eof()) {
      ss >> nodeValue;
      Node<T> const& neighbord     = getOrCreateNode(nodeValue, graph, nodes);
      nodesInfo[neighbord.nNodeId] = neighbord;
      graph.addEdge(node.nNodeId, neighbord.nNodeId);
    }
  }
}

int main(int argc, char* argv[])
{
  std::map<IGraph::NodeId, Node<std::string>> nodesInfo;
  AdjancencyVectorGraph graph;
  buildGraph(graph, nodesInfo);

  std::cout << "\n" << generateDot(graph) << std::endl;
  return 0;
}
