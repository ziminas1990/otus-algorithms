#pragma once

#include <stdint.h>
#include <string>
#include <map>

#include "IGraph.h"
#include "Utils.h"
#include <sstream>

class IGraphData
{
public:
  virtual ~IGraphData() = default;

  virtual uint32_t getEdgeColor(NodeId nOneNodeId, NodeId nOtherNodeId) const = 0;
};


template<typename T>
class GraphData : public IGraphData
{
public:

  void setEdgeColor(NodeId nOneNodeId, NodeId nOtherNodeId, uint32_t nARGB)
  {
    m_colors[key(nOneNodeId, nOtherNodeId)] = nARGB;
  }

  uint32_t getEdgeColor(NodeId nOneNodeId, NodeId nOtherNodeId) const override
  {
    const uint32_t nBlack = 0x000000;
    auto I = m_colors.find(key(nOneNodeId, nOtherNodeId));
    return I != m_colors.end() ? I->second : nBlack;
  }

private:
  inline std::pair<NodeId, NodeId> key(NodeId nFirst, NodeId nSecond) const
  {
    return std::make_pair(std::min(nFirst, nSecond), std::max(nFirst, nSecond));
  }

private:
  std::map<std::pair<NodeId, NodeId>, uint32_t> m_colors;
};
