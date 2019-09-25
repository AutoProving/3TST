#pragma once

#include <algorithm>
#include <climits> // this is for the maximum size of an integer (maximum size of a weight)
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include <assert.h>

#define get_weight(x) (x >> 32)
#define get_vertex(x) (x & 4294967295)
#define merge(w,v) (((unsigned long int) w << 32) + v)

using namespace std;

typedef unsigned int Weight;

/* global variable that defines the maximum value of a weight. This should be
   undertood as infinity. this variable is initialized in file graph.cpp */
extern Weight MAX_WEIGHT;
typedef int Vertex;

void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              Vertex source);
Vertex dijkstra(const vector<map<Vertex, Weight>> &adjList,
                vector<Weight> &min_distance, vector<Vertex> &origin,
                Vertex source, const vector<int> &terminalsMap, int T);
void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              Vertex source, Vertex v1, Vertex v2);
void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              Vertex source, Vertex target);
Vertex dijkstra(const vector<map<Vertex, Weight>> &adjList,
                vector<Weight> &min_distance, vector<Vertex> &origin,
                vector<bool> &inTree,
                set<pair<Weight, Vertex>> &active_vertices);
void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              set<pair<Weight, Vertex>> &active_vertices);

class Graph {
public:
  // A graph is represented by its adjacency list.
  // a weighted edge (i,j,w) belongs to the graph if the pair (j,w) belongs to
  // adjList[i].
  int numberVertices;
  int numberEdges;
  int numberTerminals;
  vector<std::map<Vertex, Weight>> adjList;
  // the vector terminals has as many positions as terminals. position i is the
  // i-th terminal
  vector<int> terminals;
  // the vector terminalMap has as many positions as vertices in the graph.
  // Position i is set to -1 if vertex i is not a terminal, and is set to j if
  // terminals[j] = i.
  vector<int> terminalsMap;

  explicit Graph(istream &input);
  void print();
  bool find_next(std::istream &input, const std::string &pat,
                 std::string &line);

  // contract node of degree 2 and return a map which store the contracted path
  map<pair<Vertex, Vertex>, vector<Vertex>> contract();
};

void check(const Graph &G);
