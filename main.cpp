#include <csignal>
#include <iostream>

#include "graph.hpp"
#include "heuristic.hpp"
#include "tree.hpp"

using namespace std;

void usage(char *name) {
  cout << "Usage: " << name << " [OPTIONS] " << endl << endl;

  cout << name
       << " is a heuristic for the Steiner tree problem. It reads the input on\
 stdin. By default, "
       << name << " takes a graph as input and run a deterministic\
 heuristic on it and output a Steiner tree."
       << endl;
  cout << name << " can be stop by sending an SIGINT or SIGTERM" << endl
       << endl;
  cout << "OPTIONS:\n\
    -r, --random\n\
        After running the deterministic heuristic, it will run some randomised procedure. This does not terminate by itself and need to be explicitly stopped by sending to it a SIGINT or SIGTERM.\n\
    -i, --improve\n\
        In addition to the graph, takes a Steiner tree as input and tries to improve it. This is a deterministic procedure. With this option --random and --seed have no effect.\n\
    -s SEED, --seed SEED\n\
        Initialize the random number generator with SEED. SEED must be an integer.\n\
        Useful with the --random option.\
" << endl;
}

int main(int argc, char **argv) {
  // register signal SIGINT and signal handler
  signal(SIGINT, signalHandler);
  signal(SIGTERM, signalHandler);

  bool improve = false;
  bool random_init = false;

  for (auto count = 1; count < argc; ++count) {
    string arg(argv[count]);
    if (arg == "-h" || arg == "--help") {
      usage(argv[0]);
      return 0;
    }
    if (arg == "-s" || arg == "--seed") {
      try {
        ++count;
        if (count >= argc) throw invalid_argument("Missing seed");
        srand(stoi(argv[count]));
      } catch (const invalid_argument &e) {
        cerr << "Invalid argument: seed must be an integer." << endl;
        usage(argv[0]);
        return 1;
      }
      continue;
    }
    if (arg == "-i" || arg == "--improve") {
      improve = true;
      continue;
    }
    if (arg == "-r" || arg == "--random") {
      random_init = true;
      continue;
    }
    cerr << "Invalid argument: " << arg << endl;
    usage(argv[0]);
    return 1;
  }

  Graph G(cin);
  if (not improve) {
    map<pair<Vertex, Vertex>, vector<Vertex>> hash = G.contract();
    pair<Tree, Weight> p =
        complet_heuristic(G, G.terminalsMap, G.terminals, random_init);
    cout << "VALUE " << p.second << endl;
    p.first.print(hash);
  } else {
    Tree T(G, cin);
    Weight w, oldw;
    w = T.pruneLeaves(G.terminalsMap);
    do {
      if (tle)
        break;
      oldw = w;
      apply_opt(T, G);
      w = T.pruneLeaves(G.terminalsMap);
    } while (oldw != w);
    do {
      if (tle)
        break;
      oldw = w;
      full_d3(T, G);
      w = T.pruneLeaves(G.terminalsMap);
    } while (oldw != w);
    cout << "VALUE " << w << endl;
    T.print();
  }

  return 0;
}
