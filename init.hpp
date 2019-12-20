#pragma once

#include <stack>
#include <random>

#include "graph.hpp"
#include "opt.hpp"
#include "tree.hpp"
#include "dijkstra.hpp"

Tree incrementalDijks3(const Graph &G, Vertex root,
                       const vector<int> &terminalsMap,
                       const vector<Vertex> &terminals);
Tree incrementalOptDijks3(const Graph &G, Vertex root,
                          const vector<int> &terminalsMap,
                          const vector<Vertex> &terminals);
Tree mst(const Graph &g, Vertex root);
Tree strong_bias_random(const Graph &g, Vertex root);
Tree random(const Graph &g, Vertex root);
