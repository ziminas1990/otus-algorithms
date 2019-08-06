#pragma once

#include <stdint.h>
#include <string>
#include <map>
#include <vector>

#include "IGraph.h"
#include "Utils.h"
#include <sstream>

class IGraphData
{
public:
  virtual ~IGraphData() = default;

  using Levels = std::vector<NodesVector>;

  virtual std::string   getNodeName(IGraph::NodeId const& nNodeId) const = 0;
  virtual uint32_t      getNodeColorARGB(IGraph::NodeId const& nNodeId) const = 0;
  virtual Levels const& getLevels() const = 0;
};


template<typename T>
class GraphData : public IGraphData
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

  uint32_t getNodeColorARGB(IGraph::NodeId const& /*nNodeId*/) const override
  {
    static const uint32_t nGrey = 0x00A0A0A0;
    return nGrey;
  }

  Levels const& getLevels() const override { return m_levels; }


  void setLevels(Levels levels) { m_levels = std::move(levels); }

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

private:
  std::vector<Node>           m_nodes;
  std::map<T, IGraph::NodeId> m_valueToNodeId;

  Levels m_levels;
};
