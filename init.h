#pragma once

#include <stack>

#include "graph.h"
#include "opt.h"
#include "tree.h"

Tree spanningTree(Graph &G, Vertex root);
Tree incrementalDijks3(const Graph &G, Vertex root,
                       const vector<int> &terminalsMap,
                       const vector<Vertex> &terminals);
Tree incrementalOptDijks3(const Graph &G, Vertex root,
                          const vector<int> &terminalsMap,
                          const vector<Vertex> &terminals);
