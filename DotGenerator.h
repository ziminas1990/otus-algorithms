#pragma once

#include <string>
#include "IGraph.h"

class INodeStorage
{
public:
  virtual ~INodeStorage() = default;

  virtual std::string getNodeName(IGraph::NodeId const& nNodeId) const = 0;
};

std::string generateDot(IGraph const& graph, INodeStorage const& data);
