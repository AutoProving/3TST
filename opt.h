#pragma once

#include <csignal>
#include <queue>

#include "init.h"
#include "tree.h"

extern volatile sig_atomic_t tle;

void signalHandler(int);

void apply_opt(Tree &T);
void full_d3(Tree &T);

pair<Tree, Weight> complet_heuristic(const Graph &G,
                                     const vector<int> &terminalsMap,
                                     const vector<Vertex> &terminals);
