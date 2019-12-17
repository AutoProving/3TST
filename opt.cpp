#include "opt.hpp"

volatile sig_atomic_t tle = false;

void signalHandler(int) { tle = true; }

inline void initPriorityQueue(const Tree &T, const Vertex root,
                              const vector<map<Vertex, Weight>> &adjList,
                              vector<Weight> &min_distance,
                              vector<Vertex> &origin,
                              set<pair<Weight, Vertex>> &active_vertices) {
  queue<Vertex> next;
  next.push(root);
  while (!next.empty()) {
    Vertex current = next.front();
    next.pop();

    for (map<Vertex, Weight>::const_iterator it = adjList[current].begin();
         it != adjList[current].end(); ++it) {
      set<Vertex>::const_iterator son =
          T.tree[current].children.find(it->first);
      if (son != T.tree[current].children.end()) {
        next.push(*son);
        active_vertices.erase({min_distance[it->first], it->first});
        min_distance[it->first] = 0;
        origin[it->first] = current;
      } else if (min_distance[it->first] > it->second) {
        active_vertices.erase({min_distance[it->first], it->first});
        active_vertices.insert({it->second, it->first});
        min_distance[it->first] = it->second;
        origin[it->first] = current;
      }
    }
  }
}

inline Vertex deleteUpPath(Vertex v, const vector<int> &terminalsMap, Tree &T) {
  while (T.tree[v].children.empty() && terminalsMap[v] == -1 &&
         T.tree[v].parent >= 0) {
    Vertex parent = T.tree[v].parent;
    T.tree[parent].children.erase(v);
    T.tree[v].parent = -2;
    v = parent;
  }
  return v;
}

inline void buildDownPath(const vector<map<Vertex, Weight>> &adjList,
                          const vector<Weight> &min_distance,
                          const vector<Vertex> &origin, Vertex cible, Tree &T) {
  while (origin[cible] != -1) {
    if (T.tree[origin[cible]].parent >= 0) {
      T.tree[T.tree[origin[cible]].parent].children.erase(origin[cible]);
    }
    T.tree[cible].children.insert(origin[cible]);
    T.tree[origin[cible]].parent = cible;
    if (T.tree[origin[cible]].parent == -2) {
      T.tree[origin[cible]].weight =
          min_distance[cible] - min_distance[origin[cible]];
    } else {
      T.tree[origin[cible]].weight = adjList[cible].find(origin[cible])->second;
    }
    cible = origin[cible];
  }
}

inline void buildUpPath(const vector<map<Vertex, Weight>> &adjList,
                        const vector<Weight> &min_distance,
                        const vector<Vertex> &origin, Vertex current, Tree &T) {
  while (origin[current] != -1) {
    if (T.tree[current].parent == origin[current]) {
      current = origin[current];
    } else {
      if (T.tree[current].parent >= 0)
        T.tree[T.tree[current].parent].children.erase(current);
      T.tree[origin[current]].children.insert(current);
      T.tree[current].parent = origin[current];
      if (T.tree[current].parent >= 0) {
        T.tree[current].weight =
            min_distance[current] - min_distance[origin[current]];
      } else {
        T.tree[current].weight = adjList[current].find(origin[current])->second;
      }
      current = origin[current];
    }
  }
}

inline void opt_d3(Vertex root, Vertex v1, Vertex v2,
                   const vector<map<Vertex, Weight>> &adjList,
                   vector<Vertex> terminalsMap, Tree &T) {

  if (T.tree[v1].parent >= 0) {
    T.tree[T.tree[v1].parent].children.erase(v1);
    deleteUpPath(T.tree[v1].parent, terminalsMap, T);
  }
  if (T.tree[v2].parent >= 0) {
    T.tree[T.tree[v2].parent].children.erase(v2);
    deleteUpPath(T.tree[v2].parent, terminalsMap, T);
  }

  while (T.tree[v1].children.size() == 1 && terminalsMap[v1] == -1) {
    T.tree[v1].parent = -2;
    Vertex tmp = v1;
    v1 = *T.tree[v1].children.begin();
    T.tree[tmp].children.clear();
  }
  T.tree[v1].parent = -1;

  while (T.tree[v2].children.size() == 1 && terminalsMap[v2] == -1) {
    T.tree[v2].parent = -2;
    Vertex tmp = v2;
    v2 = *T.tree[v2].children.begin();
    T.tree[tmp].children.clear();
  }
  T.tree[v2].parent = -1;

  // Dijkstra from v1
  set<pair<Weight, Vertex>> active_vertices;
  vector<Weight> min_distance1(adjList.size(), MAX_WEIGHT);
  vector<Vertex> origin1(adjList.size(), -1);
  min_distance1[v1] = 0;

  initPriorityQueue(T, v1, adjList, min_distance1, origin1, active_vertices);
  Vertex c1 = dijkstra(adjList, min_distance1, origin1, T, active_vertices);

  // Dijkstra from v2
  active_vertices.clear();
  vector<Weight> min_distance2(adjList.size(), MAX_WEIGHT);
  vector<Vertex> origin2(adjList.size(), -1);
  min_distance2[v2] = 0;

  initPriorityQueue(T, v2, adjList, min_distance2, origin2, active_vertices);
  Vertex c2 = dijkstra(adjList, min_distance2, origin2, T, active_vertices);

  // Dijkstra from root
  active_vertices.clear();
  vector<Weight> min_distance0(adjList.size(), MAX_WEIGHT);
  vector<Vertex> origin0(adjList.size(), -1);
  min_distance0[root] = 0;

  initPriorityQueue(T, root, adjList, min_distance0, origin0, active_vertices);
  Vertex c0 = dijkstra(adjList, min_distance0, origin0, T, active_vertices);

  Vertex r1;
  if (min_distance2[c1] == 0) {
    r1 = v2;
  } else {
    r1 = root;
  }
  Vertex r2;
  if (min_distance1[c2] == 0) {
    r2 = v1;
  } else {
    r2 = root;
  }
  Vertex r0;
  if (min_distance1[c0] == 0) {
    r0 = v1;
  } else {
    r0 = v2;
  }

  Vertex intersect = -1;
  Weight distIntersect = MAX_WEIGHT;
  if (r1 == root || r2 == root) {
    distIntersect = min_distance1[c1] + min_distance2[c2];
  } else if (r0 == v1) {
    distIntersect = min_distance0[c0] + min_distance2[c2];
  } else {
    distIntersect = min_distance0[c0] + min_distance1[c1];
  }
  for (unsigned int i = 0; i < adjList.size(); ++i) {
    if (min_distance2[i] != MAX_WEIGHT && min_distance1[i] != MAX_WEIGHT &&
        min_distance0[i] != MAX_WEIGHT &&
        min_distance0[i] + min_distance1[i] + min_distance2[i] <
            distIntersect) {
      intersect = i;
      distIntersect = min_distance0[i] + min_distance1[i] + min_distance2[i];
    }
  }

  if (intersect != -1) {
    // Build the tree with origin1
    buildDownPath(adjList, min_distance1, origin1, intersect, T);

    // Build the tree with origin2
    buildDownPath(adjList, min_distance2, origin2, intersect, T);

    // Build the tree with origin0
    buildUpPath(adjList, min_distance0, origin0, intersect, T);
  } else {
    if (r2 == root) {
      // Build the tree with origin1
      buildDownPath(adjList, min_distance1, origin1, c1, T);
      // Build the tree with origin2
      buildDownPath(adjList, min_distance2, origin2, c2, T);
    } else if (r1 == root) {
      // Build the tree with origin2
      buildDownPath(adjList, min_distance2, origin2, c2, T);
      // Build the tree with origin1
      buildDownPath(adjList, min_distance1, origin1, c1, T);
    } else {
      // Build the tree with origin1
      if (r0 == v1) {
        // Build the tree with origin2
        buildDownPath(adjList, min_distance2, origin2, c2, T);
        // Build the tree with origin1
        buildDownPath(adjList, min_distance1, origin1, c0, T);
      } else {
        // Build the tree with origin1
        buildDownPath(adjList, min_distance1, origin1, c1, T);
        // Build the tree with origin2
        buildDownPath(adjList, min_distance2, origin2, c0, T);
      }

      // Build the tree with origin0
      buildUpPath(adjList, min_distance0, origin0, c0, T);
    }
  }
}

inline void opt_d3_parent(Vertex node, Tree &T,
                          const vector<map<Vertex, Weight>> &adjList,
                          const vector<int> &terminalsMap) {
  vector<Vertex> backup_children(T.tree[node].children.begin(),
                                 T.tree[node].children.end());
  for (vector<Vertex>::iterator it1 = backup_children.begin();
       it1 != backup_children.end(); ++it1) {
    if (T.tree[node].parent < 0 || tle)
      return;
    set<Vertex>::iterator tmp_it = T.tree[node].children.find(*it1);
    if (tmp_it == T.tree[node].children.end())
      return;
    Vertex v1 = *tmp_it;
    Vertex v2 = node;

    opt_d3(T.root, v1, v2, adjList, terminalsMap, T);
  }
}

inline void opt_d3_son(Vertex node, Tree &T,
                       const vector<map<Vertex, Weight>> &adjList,
                       const vector<int> &terminalsMap) {
  vector<Vertex> backup_children(T.tree[node].children.begin(),
                                 T.tree[node].children.end());
  for (vector<Vertex>::iterator it1 = backup_children.begin();
       it1 != backup_children.end(); ++it1) {
    if (T.tree[node].parent == -2)
      return;
    for (vector<Vertex>::iterator it2 = it1 + 1; it2 != backup_children.end();
         ++it2) {
      if (tle)
        return;
      if (T.tree[node].parent == -2)
        break;
      if (T.tree[node].children.find(*it1) == T.tree[node].children.end())
        break;
      if (T.tree[node].children.find(*it2) == T.tree[node].children.end())
        continue;

      opt_d3(T.root, *it1, *it2, adjList, terminalsMap, T);
    }
  }
}

void apply_opt(Tree &T, const Graph &G) {
  vector<Vertex> next_opt;
  next_opt.reserve(G.adjList.size());
  {
    queue<Vertex> next;
    next.push(T.root);

    // We start the optimisation by the leaves
    // Fill next_opt with the order
    while (!next.empty()) {
      Vertex current = next.front();
      next.pop();

      for (set<Vertex>::iterator it = T.tree[current].children.begin();
           it != T.tree[current].children.end(); ++it) {
        next.push(*it);
        if (tle)
          return;
      }
      next_opt.push_back(current);
    }
  }

  // apply optimisation following the next_opt order
  for (vector<Vertex>::reverse_iterator it = next_opt.rbegin();
       it != next_opt.rend(); ++it) {
    if (T.tree[*it].parent > -2) {

      // Optimisation for node of degree 3 or more
      if (tle)
        return;
      if (T.tree[*it].children.size() > 1) {
        opt_d3_son(*it, T, G.adjList, G.terminalsMap);
        T.check(G);
      }
      if (tle)
        return;
      if (T.tree[*it].children.size() > 2 ||
          (T.tree[*it].children.size() == 2 && G.terminalsMap[*it] != -1)) {
        opt_d3_parent(*it, T, G.adjList, G.terminalsMap);
        T.check(G);
      }
    }
  }
}

void full_d3(Tree &T, const Graph &G) {
  vector<Vertex> next_opt;
  next_opt.reserve(G.adjList.size());
  {
    queue<Vertex> next;
    next.push(T.root);

    while (!next.empty()) {
      Vertex current = next.front();
      next.pop();

      for (set<Vertex>::iterator it = T.tree[current].children.begin();
           it != T.tree[current].children.end(); ++it) {
        if (T.tree[*it].children.size() > 1 || G.terminalsMap[*it])
          next_opt.push_back(*it);
        next.push(*it);
        if (tle)
          return;
      }
    }
  }

  for (vector<Vertex>::iterator it1 = next_opt.begin(); it1 != next_opt.end();
       ++it1) {
    for (vector<Vertex>::iterator it2 = it1 + 1; it2 != next_opt.end(); ++it2) {
      if (tle)
        return;
      if (T.tree[*it1].parent != -2) {
        if (T.tree[*it2].parent != -2) {
          if ((T.tree[*it1].children.size() > 1 ||
               G.terminalsMap[*it1] != -1) &&
              (T.tree[*it2].children.size() > 1 ||
               G.terminalsMap[*it2] != -1)) {
            opt_d3(T.root, *it1, *it2, G.adjList, G.terminalsMap, T);
          }
        }
      }
    }
  }
}
