#include "heuristic.hpp"

using namespace std::chrono;

Graph merge_trees(const vector<Tree> &trees, const vector<int> &terminals,
                  const vector<int> &terminalsMap) {
  Graph G(terminals, terminalsMap);
  for (Tree T : trees) {
    for (unsigned int i = 0; i < T.tree.size(); ++i) {
      if (T.tree[i].parent >= 0) {
        G.adjList[i].insert({T.tree[i].parent, T.tree[i].weight});
        G.adjList[T.tree[i].parent].insert({i, T.tree[i].weight});
      }
    }
  }
  return G;
}

Weight complet_opt(Tree &T, const Graph &G, const vector<int> &terminalsMap) {
  Weight oldw, w = T.pruneLeaves(terminalsMap);
  do {
    if (tle)
      break;
    oldw = w;
    apply_opt(T, G);
    w = T.pruneLeaves(terminalsMap);
  } while (oldw != w);
  do {
    if (tle)
      break;
    oldw = w;
    full_d3(T, G);
    w = T.pruneLeaves(terminalsMap);
  } while (oldw != w);
  return w;
}

pair<Tree, Weight> complet_heuristic(const Graph &G,
                                     const vector<int> &terminalsMap,
                                     const vector<Vertex> &terminals,
                                     bool random_init,
                                     const time_point<high_resolution_clock> &general_start) {
  switch (terminals.size()) {
  case 0:
    return {Tree(G, 0), 0};
  case 1:
    return {Tree(G, terminals[0]), 0};
  case 2: {
    Tree T = incrementalDijks3(G, terminals[0], terminalsMap, terminals);
    return {T, T.pruneLeaves(terminalsMap)};
  }
  case 3: {
    Tree T = incrementalDijks3(G, terminals[0], terminalsMap, terminals);
    return {T, T.pruneLeaves(terminalsMap)};
  }
  default: {
    Tree T(G, 0);
    Weight w, wf = MAX_WEIGHT;
    for (vector<Vertex>::const_iterator it = terminals.begin();
         it != terminals.end(); ++it) {
      if (tle)
        break;
      auto start = high_resolution_clock::now();
      Tree tmp = incrementalDijks3(G, *it, terminalsMap, terminals);
      auto stop = high_resolution_clock::now();
      auto duration = duration_cast<microseconds>(stop - start);
      cerr << "IncremetnalDijks3: " << duration.count() << " "
           << tmp.pruneLeaves(terminalsMap) << endl;
      start = high_resolution_clock::now();
      w = complet_opt(tmp, G, terminalsMap);
      stop = high_resolution_clock::now();
      duration = duration_cast<microseconds>(stop - start);
      cerr << "complet_opt: " << duration.count() << " " << w << endl;
      if (wf > w) {
        // TODO log best solution with time
        wf = w;
        T.root = tmp.root;
        T.tree.swap(tmp.tree);
        duration = duration_cast<microseconds>(stop - general_start);
        cerr << "new_opt: " << duration.count() << " " << wf << endl;
      }
    }
    while (!tle && random_init) {
      for (vector<Vertex>::const_iterator it = terminals.begin();
           it != terminals.end(); ++it) {
        if (tle)
          break;
        auto start = high_resolution_clock::now();
        Tree tmp = random(G, *it);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(stop - start);
        cerr << "random: " << duration.count() << " "
            << tmp.pruneLeaves(terminalsMap) << endl;
        start = high_resolution_clock::now();
        w = complet_opt(tmp, G, terminalsMap);
        stop = high_resolution_clock::now();
        duration = duration_cast<microseconds>(stop - start);
        cerr << "complet_opt_random: " << duration.count() << " " << w << endl;
        if (wf > w) {
          wf = w;
          T.root = tmp.root;
          T.tree.swap(tmp.tree);
          duration = duration_cast<microseconds>(stop - general_start);
          cerr << "new_opt: " << duration.count() << " " << wf << endl;
        }
      }
    }
    return {T, wf};
  }
  }
}
