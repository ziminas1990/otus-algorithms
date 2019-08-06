#pragma once

#include <string>
#include "IGraph.h"
#include "GraphData.h"

std::string generateGraphviz(IGraph const& graph, IGraphData const& data);
