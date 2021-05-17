#pragma once

#include <algorithm>
#include <assert.h>
#include <climits> // this is for the maximum size of an integer (maximum size of a weight)
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>

using namespace std;

typedef long int Weight;

/* Global variable that defines the maximum value of a weight. This should be
   understood as infinity. This variable is initialized in the file graph.cpp */
extern Weight MAX_WEIGHT;
typedef int Vertex;

class Graph {
public:
  // A graph is represented by its adjacency list.
  // A weighted edge (i,j,w) belongs to the graph if the pair (j,w) belongs to
  // adjList[i].
  vector<std::map<Vertex, Weight>> adjList;
  // The vector terminals has as many positions as terminals. Position i is the
  // i-th terminal
  vector<int> terminals;
  // The vector terminalMap has as many positions as vertices in the graph.
  // Position i is set to -1 if vertex i is not a terminal, and is set to j if
  // terminals[j] = i.
  vector<int> terminalsMap;

  Graph(vector<int> terminals, vector<int> terminalsMap)
      : adjList{vector<std::map<Vertex, Weight>>(terminalsMap.size())},
        terminals{terminals}, terminalsMap{terminalsMap} {};
  explicit Graph(istream &input);
  void print();
  bool find_next(std::istream &input, const std::string &pat,
                 std::string &line);

  // Contracts nodes of degree 2 and returns a map which store the contracted paths
  map<pair<Vertex, Vertex>, vector<Vertex>> contract();
};
