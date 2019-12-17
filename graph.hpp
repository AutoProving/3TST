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

/* global variable that defines the maximum value of a weight. This should be
   undertood as infinity. this variable is initialized in file graph.cpp */
extern Weight MAX_WEIGHT;
typedef int Vertex;

class Graph {
public:
  // A graph is represented by its adjacency list.
  // a weighted edge (i,j,w) belongs to the graph if the pair (j,w) belongs to
  // adjList[i].
  vector<std::map<Vertex, Weight>> adjList;
  // the vector terminals has as many positions as terminals. position i is the
  // i-th terminal
  vector<int> terminals;
  // the vector terminalMap has as many positions as vertices in the graph.
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

  // contract node of degree 2 and return a map which store the contracted path
  map<pair<Vertex, Vertex>, vector<Vertex>> contract();
};
