#pragma once

#include <string>
#include "IGraph.h"

class INodeStorage
{
public:
  virtual ~INodeStorage() = default;

  virtual std::string getNodeName(IGraph::NodeId const& nNodeId) = 0;
  virtual uint32_t    getNodeColor(IGraph::NodeId const& nNodeId) = 0;
};

std::string generateDot(IGraph const& graph);
