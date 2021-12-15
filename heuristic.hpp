#pragma once

#include <chrono>

#include "init.hpp"
#include "opt.hpp"

pair<Tree, Weight> complet_heuristic(
    const Graph &G, const vector<int> &terminalsMap,
    const vector<Vertex> &terminals, bool random_init,
    const std::chrono::time_point<std::chrono::high_resolution_clock>
        &general_start);
