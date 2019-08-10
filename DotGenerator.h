#pragma once

#include <string>
#include "IGraph.h"
#include "GraphData.h"

std::string generateGraphviz(const IWeightedGraph &graph, IGraphData const& data);
