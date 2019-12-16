#include <csignal>
#include <iostream>

#include "graph.hpp"
#include "heuristic.hpp"
#include "tree.hpp"

using namespace std;

int main(int argc, char **argv) {
  // register signal SIGINT and signal handler
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  Graph G(cin);
  if (argc < 4) {
    map<pair<Vertex, Vertex>, vector<Vertex>> hash = G.contract();
    pair<Tree, Weight> p = complet_heuristic(G, G.terminalsMap, G.terminals);
    cout << "VALUE " << p.second << endl;
    p.first.print(hash);
  } else {
    Tree T(G, cin);
    Weight w, oldw = MAX_WEIGHT;
    w = T.pruneLeaves(G.terminalsMap);
    do {
      if (tle)
        break;
      oldw = w;
      apply_opt(T,G);
      w = T.pruneLeaves(G.terminalsMap);
    } while (oldw != w);
    do {
      if (tle)
        break;
      oldw = w;
      full_d3(T,G);
      w = T.pruneLeaves(G.terminalsMap);
    } while (oldw != w);
    cout << "VALUE " << w << endl;
    T.print();
  }

  return 0;
}
