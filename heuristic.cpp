#include "heuristic.hpp"

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

pair<Tree, Weight> complet_heuristic(const Graph &G,
                                     const vector<int> &terminalsMap,
                                     const vector<Vertex> &terminals) {
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
    Weight w, oldw, wf = MAX_WEIGHT;
    for (vector<Vertex>::const_iterator it = terminals.begin();
         it != terminals.end(); ++it) {
      if (tle)
        break;
      Tree tmp = incrementalDijks3(G, *it, terminalsMap, terminals);
      w = tmp.pruneLeaves(terminalsMap);
      do {
        if (tle)
          break;
        oldw = w;
        apply_opt(tmp, G);
        w = tmp.pruneLeaves(terminalsMap);
      } while (oldw != w);
      do {
        if (tle)
          break;
        oldw = w;
        full_d3(tmp, G);
        w = tmp.pruneLeaves(terminalsMap);
      } while (oldw != w);
      if (wf > w) {
        wf = w;
        T.root = tmp.root;
        T.tree.swap(tmp.tree);
      }
    }
    for (vector<Vertex>::const_iterator it = terminals.begin();
         it != terminals.end(); ++it) {
      if (tle)
        break;
      Tree tmp = incrementalOptDijks3(G, *it, terminalsMap, terminals);
      w = tmp.pruneLeaves(G.terminalsMap);
      do {
        if (tle)
          break;
        oldw = w;
        apply_opt(tmp, G);
        w = tmp.pruneLeaves(G.terminalsMap);
      } while (oldw != w);
      do {
        if (tle)
          break;
        oldw = w;
        full_d3(tmp, G);
        w = tmp.pruneLeaves(G.terminalsMap);
      } while (oldw != w);
      if (wf > w) {
        wf = w;
        T.root = tmp.root;
        T.tree.swap(tmp.tree);
      }
    }
    return {T, wf};
  }
  }
}
