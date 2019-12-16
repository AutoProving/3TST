#pragma once
#include "tree.hpp"
#include "graph.hpp"

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
                vector<Weight> &min_distance, vector<Vertex> &origin, Tree &T,
                set<pair<Weight, Vertex>> &active_vertices);
void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              set<pair<Weight, Vertex>> &active_vertices);

