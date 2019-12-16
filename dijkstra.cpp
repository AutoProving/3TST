#include "dijkstra.hpp"

bool cmp(const pair<Weight, Vertex> &x, const pair<Weight, Vertex> &y) {
  return x.first > y.first;
}

void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              Vertex source) {
  min_distance[source] = 0;
  vector<pair<Weight, Vertex>> active_vertices;
  active_vertices.reserve(adjList.size());
  active_vertices.push_back({0, source});
  vector<bool> notseen(adjList.size(), true);

  while (!active_vertices.empty()) {
    std::pop_heap(active_vertices.begin(), active_vertices.end(), cmp);
    Vertex where = active_vertices.back().second;
    active_vertices.pop_back();
    if (notseen[where]) {
      for (std::map<Vertex, Weight>::const_iterator it = adjList[where].begin();
           it != adjList[where].end(); ++it)
        if (min_distance[it->first] > min_distance[where] + it->second) {
          min_distance[it->first] = min_distance[where] + it->second;
          origin[it->first] = where;
          active_vertices.push_back({min_distance[it->first], it->first});
          push_heap(active_vertices.begin(), active_vertices.end(), cmp);
        }
      notseen[where] = false;
    }
  }
}

void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              set<pair<Weight, Vertex>> &set_active_vertices) {

  vector<pair<Weight, Vertex>> active_vertices;
  active_vertices.reserve(adjList.size());
  active_vertices.insert(active_vertices.begin(), set_active_vertices.begin(),
                         set_active_vertices.end());
  vector<bool> notseen(adjList.size(), true);
  make_heap(active_vertices.begin(), active_vertices.end(), cmp);
  while (!active_vertices.empty()) {
    std::pop_heap(active_vertices.begin(), active_vertices.end(), cmp);
    Vertex where = active_vertices.back().second;
    active_vertices.pop_back();
    if (notseen[where]) {
      for (std::map<Vertex, Weight>::const_iterator it = adjList[where].begin();
           it != adjList[where].end(); ++it)
        if (min_distance[it->first] > min_distance[where] + it->second) {
          min_distance[it->first] = min_distance[where] + it->second;
          origin[it->first] = where;
          active_vertices.push_back({min_distance[it->first], it->first});
          push_heap(active_vertices.begin(), active_vertices.end(), cmp);
        }
      notseen[where] = false;
    }
  }
}

Vertex dijkstra(const vector<map<Vertex, Weight>> &adjList,
                vector<Weight> &min_distance, vector<Vertex> &origin,
                Vertex source, const vector<int> &terminalsMap, int T) {
  min_distance[source] = 0;
  vector<pair<Weight, Vertex>> active_vertices;
  active_vertices.reserve(adjList.size());
  active_vertices.push_back({0, source});
  vector<bool> notseen(adjList.size(), true);

  while (!active_vertices.empty()) {
    std::pop_heap(active_vertices.begin(), active_vertices.end(), cmp);
    Vertex where = active_vertices.back().second;
    active_vertices.pop_back();
    if (notseen[where]) {
      if (terminalsMap[where] != -1)
        --T;
      if (T == 0)
        return where;
      for (std::map<Vertex, Weight>::const_iterator it = adjList[where].begin();
           it != adjList[where].end(); ++it)
        if (min_distance[it->first] > min_distance[where] + it->second) {
          min_distance[it->first] = min_distance[where] + it->second;
          origin[it->first] = where;
          active_vertices.push_back({min_distance[it->first], it->first});
          push_heap(active_vertices.begin(), active_vertices.end(), cmp);
        }
      notseen[where] = false;
    }
  }
  return -1;
}

Vertex dijkstra(const vector<map<Vertex, Weight>> &adjList,
                vector<Weight> &min_distance, vector<Vertex> &origin,
                Tree &T,
                set<pair<Weight, Vertex>> &set_active_vertices) {

  vector<pair<Weight, Vertex>> active_vertices;
  active_vertices.reserve(adjList.size());
  active_vertices.insert(active_vertices.begin(), set_active_vertices.begin(),
                         set_active_vertices.end());
  vector<bool> notseen(adjList.size(), true);
  make_heap(active_vertices.begin(), active_vertices.end(), cmp);
  while (!active_vertices.empty()) {
    std::pop_heap(active_vertices.begin(), active_vertices.end(), cmp);
    Vertex where = active_vertices.back().second;
    active_vertices.pop_back();
    if (notseen[where]) {
      if (T.tree[where].parent > -2)
        return where;
      for (std::map<Vertex, Weight>::const_iterator it = adjList[where].begin();
           it != adjList[where].end(); ++it)
        if (min_distance[it->first] > min_distance[where] + it->second) {
          min_distance[it->first] = min_distance[where] + it->second;
          origin[it->first] = where;
          active_vertices.push_back({min_distance[it->first], it->first});
          push_heap(active_vertices.begin(), active_vertices.end(), cmp);
        }
      notseen[where] = false;
    }
  }
  return -1;
}

void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              Vertex source, Vertex target) {
  min_distance[source] = 0;
  vector<pair<Weight, Vertex>> active_vertices;
  active_vertices.reserve(adjList.size());
  active_vertices.push_back({0, source});
  vector<bool> notseen(adjList.size(), true);

  while (!active_vertices.empty()) {
    std::pop_heap(active_vertices.begin(), active_vertices.end(), cmp);
    Vertex where = active_vertices.back().second;
    active_vertices.pop_back();
    if (notseen[where]) {
      if (where == target)
        return;
      for (std::map<Vertex, Weight>::const_iterator it = adjList[where].begin();
           it != adjList[where].end(); ++it)
        if (min_distance[it->first] > min_distance[where] + it->second) {
          min_distance[it->first] = min_distance[where] + it->second;
          origin[it->first] = where;
          active_vertices.push_back({min_distance[it->first], it->first});
          push_heap(active_vertices.begin(), active_vertices.end(), cmp);
        }
      notseen[where] = false;
    }
  }
}

void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              Vertex source, Vertex v1, Vertex v2) {
  min_distance[source] = 0;
  vector<pair<Weight, Vertex>> active_vertices;
  active_vertices.reserve(adjList.size());
  active_vertices.push_back({0, source});
  vector<bool> notseen(adjList.size(), true);

  while (!active_vertices.empty()) {
    std::pop_heap(active_vertices.begin(), active_vertices.end(), cmp);
    Vertex where = active_vertices.back().second;
    active_vertices.pop_back();
    if (notseen[where]) {
      if (where == v1 || where == v2)
        return;
      for (std::map<Vertex, Weight>::const_iterator it = adjList[where].begin();
           it != adjList[where].end(); ++it)
        if (min_distance[it->first] > min_distance[where] + it->second) {
          min_distance[it->first] = min_distance[where] + it->second;
          origin[it->first] = where;
          active_vertices.push_back({min_distance[it->first], it->first});
          push_heap(active_vertices.begin(), active_vertices.end(), cmp);
        }
      notseen[where] = false;
    }
  }
}
