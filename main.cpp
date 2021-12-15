#include <chrono>
#include <csignal>
#include <iostream>

#include "graph.hpp"
#include "heuristic.hpp"
#include "tree.hpp"

using namespace std;
using namespace std::chrono;

void usage(char *name) {
  cout << "Usage: " << name << " [OPTIONS] " << endl << endl;

  cout << name
       << " is a heuristic for the Steiner tree problem. It reads the input from\
 stdin. By default, "
       << name << " takes a graph as input and run a deterministic\
 heuristic on it and outputs a Steiner tree."
       << endl;
  cout << name << " can be stopped by a SIGINT or SIGTERM signal." << endl
       << endl;
  cout << "OPTIONS:\n\
    -r, --random\n\
        After running a deterministic heuristic to construct a reasonable initial Steiner tree,\n\
       	a randomized improvement procedure will be initiated. This procedure does not terminate by \n\
	itself and needs to be explicitly stopped by a SIGINT or SIGTERM signal.\n\
    -i, --improve\n\
        With this option, the program will take a graph and a Steiner tree as input and will \n\
	try to construct a lighter Steiner tree. This is a deterministic procedure. With this option,\n\
        the flags --random and --seed have no effect.\n\
    -s SEED, --seed SEED\n\
        Initializes the random number generator with SEED. SEED must be an integer.\n\
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
        if (count >= argc)
          throw invalid_argument("Missing seed");
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

  auto start = high_resolution_clock::now();
  Graph G(cin);
  if (not improve) {
    map<pair<Vertex, Vertex>, vector<Vertex>> hash = G.contract();
    pair<Tree, Weight> p =
        complet_heuristic(G, G.terminalsMap, G.terminals, random_init, start);
    cout << "VALUE " << p.second << endl;
    p.first.print(hash);
  } else {
    Weight w, oldw;
    Tree T(G, cin);
    w = T.pruneLeaves(G.terminalsMap);
    map<pair<Vertex, Vertex>, vector<Vertex>> hash = G.contract();
    T.contract(G);
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

  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cerr << "Full run: " << duration.count() << endl;
  return 0;
}
