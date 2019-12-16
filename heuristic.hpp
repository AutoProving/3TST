#pragma once

#include "init.hpp"
#include "opt.hpp"

pair<Tree, Weight> complet_heuristic(const Graph &G,
                                     const vector<int> &terminalsMap,
                                     const vector<Vertex> &terminals);
