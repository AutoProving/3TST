#pragma once

#include <csignal>
#include <queue>
#include <algorithm>

#include "tree.hpp"
#include "dijkstra.hpp"

extern volatile sig_atomic_t tle;

void signalHandler(int);

void apply_opt(Tree &T, const Graph &G);
void full_d3(Tree &T, const Graph &G);
