#include "init.hpp"

inline void steiner_3(const vector<map<Vertex, Weight>> &adjList,
                      vector<Weight> &min_distance0, vector<Vertex> &origin0,
                      Vertex v0, Vertex v1, Vertex v2) {
  vector<Weight> min_distance1(adjList.size(), MAX_WEIGHT);
  vector<Vertex> origin1(adjList.size(), -1);
  vector<Weight> min_distance2(adjList.size(), MAX_WEIGHT);
  vector<Vertex> origin2(adjList.size(), -1);

  dijkstra(adjList, min_distance1, origin1, v1, v0, v2);
  if (min_distance0[v2] == min_distance1[v2]) {
    dijkstra(adjList, min_distance2, origin2, v2, v0, v1);
  } else {
    if (min_distance0[v2] < min_distance1[v2]) {
      dijkstra(adjList, min_distance2, origin2, v2, v0);
    } else {
      dijkstra(adjList, min_distance2, origin2, v2, v1);
    }
  }
  Vertex intersect = -1;
  Weight distIntersect = MAX_WEIGHT;
  for (unsigned int i = 0; i < adjList.size(); ++i) {
    if (min_distance2[i] != MAX_WEIGHT && min_distance1[i] != MAX_WEIGHT &&
        min_distance0[i] != MAX_WEIGHT &&
        min_distance0[i] + min_distance1[i] + min_distance2[i] <
            distIntersect) {
      intersect = i;
      distIntersect = min_distance0[i] + min_distance1[i] + min_distance2[i];
    }
  }
  // Fill seen
  vector<bool> seen(adjList.size(),false);
  Vertex current = intersect;
  while (origin0[current] != -1) {
    seen[current] = true;
    current = origin0[current];
  }
  seen[current]=  true;
  // Merge origin1 into origin0
  current = intersect;
  while (origin1[current] != -1) {
    if (!seen[origin1[current]]) {
      origin0[origin1[current]] = current;
    }
    seen[current] = true;
    current = origin1[current];
  }
  seen[current] = true;
  // Merge origin2 into origin0
  current = intersect;
  while (origin2[current] != -1) {
    if (!seen[origin2[current]]) {
      origin0[origin2[current]] = current;
    }
    current = origin2[current];
  }
}

// TODO This can be simply by the algo from the paper. Try to rewrite it
Tree incrementalDijks3(const Graph &G, Vertex root,
                       const vector<int> &terminalsMap,
                       const vector<Vertex> &terminals) {
  vector<std::map<Vertex, Weight>> adjList = G.adjList;
  vector<Weight> min_distance(G.adjList.size(), MAX_WEIGHT);
  vector<Vertex> origin(G.adjList.size(), -1);
  Vertex far = dijkstra(adjList, min_distance, origin, root, terminalsMap,
                        terminals.size());
  Weight far_dist = min_distance[far];

  Vertex last_far = far;
  bool dijk_simple = true;
  while (far_dist > 0 && !tle) {
    if (dijk_simple) {
      dijk_simple = false;
      vector<Weight> copy_distance = min_distance;
      vector<Vertex> copy_origin = origin;

      // Update graph, set edge to 0
      last_far = far; // TODO: virer, inutile, il faut juste un valeur far pour
                      // chaque cas du if, pas besoin de copier
      Vertex current = far;
      while (origin[current] != -1 && copy_distance[current] != 0) {
        copy_distance[current] = 0;
        current = origin[current];
      }
      current = far;
      set<pair<Weight, Vertex>> active_vertices;
      while (origin[current] != -1 && min_distance[current] != 0) {
        for (map<Vertex, Weight>::const_iterator it = adjList[current].begin();
             it != adjList[current].end(); ++it) {
          if (it->first != copy_origin[current] &&
              copy_distance[it->first] > it->second) {
            active_vertices.erase({copy_distance[it->first], it->first});
            active_vertices.insert({it->second, it->first});
            copy_distance[it->first] = it->second;
            copy_origin[it->first] = current;
          }
        }
        current = origin[current];
      }
      dijkstra(adjList, copy_distance, copy_origin, active_vertices);
      far_dist = 0;
      for (vector<Vertex>::const_iterator it = terminals.begin();
           it != terminals.end(); ++it) {
        if (far_dist < copy_distance[*it]) {
          far_dist = copy_distance[*it];
          far = *it;
        }
      }
    } else {
      dijk_simple = true;
      steiner_3(adjList, min_distance, origin, root, far, last_far);

      // Update graph
      Vertex current = far;
      while (origin[current] != -1) {
        adjList[current][origin[current]] = 0;
        adjList[origin[current]][current] = 0;
        min_distance[current] = MAX_WEIGHT;
        current = origin[current];
      }
      current = last_far;
      while (origin[current] != -1) {
        adjList[current][origin[current]] = 0;
        adjList[origin[current]][current] = 0;
        min_distance[current] = MAX_WEIGHT;
        current = origin[current];
      }
      dijkstra(adjList, min_distance, origin, root);
      far_dist = 0;
      for (vector<Vertex>::const_iterator it = terminals.begin();
           it != terminals.end(); ++it) {
        if (far_dist < min_distance[*it]) {
          far_dist = min_distance[*it];
          far = *it;
        }
      }
    }
  }

  // build tree
  Tree T(G, root);
  for (unsigned int i = 0; i < G.adjList.size(); ++i) {
    if (origin[i] != -1) {
      T.tree[i].parent = origin[i];
      T.tree[i].weight = G.adjList[i].find(origin[i])->second;
      T.tree[origin[i]].children.insert(i);
    }
  }
  return T;
}

Tree incrementalOptDijks3(const Graph &G, Vertex root,
                          const vector<int> &terminalsMap,
                          const vector<Vertex> &terminals) {
  vector<std::map<Vertex, Weight>> adjList = G.adjList;
  vector<Weight> min_distance(G.adjList.size(), MAX_WEIGHT);
  vector<Vertex> origin(G.adjList.size(), -1);
  Vertex far = dijkstra(adjList, min_distance, origin, root, terminalsMap,
                        terminals.size());
  Weight far_dist = min_distance[far];

  vector<Weight> full_backup_distance = min_distance;
  vector<Vertex> full_backup_origin = origin;

  vector<Vertex> current_terminals;
  vector<int> current_terminalsMap(terminalsMap.size(), -1);
  current_terminals.reserve(terminals.size());

  Vertex last_far = far;
  while (far_dist > 0 && !tle) {
    bool dijk_simple = true;
    for (int i = 0; i < 4 && far_dist > 0 && !tle; ++i) {
      if (dijk_simple) {
        current_terminalsMap[far] = current_terminals.size();
        current_terminals.push_back(far);
        dijk_simple = false;
        vector<Weight> backup_distance = min_distance;
        vector<Vertex> backup_origin = origin;

        // Update graph, set edge to 0
        last_far = far;
        Vertex current = far;
        while (origin[current] != -1 && min_distance[current] != 0) {
          backup_distance[current] = 0;
          current = origin[current];
        }
        current = far;
        set<pair<Weight, Vertex>> active_vertices;
        while (origin[current] != -1 && min_distance[current] != 0) {
          for (map<Vertex, Weight>::iterator it = adjList[current].begin();
               it != adjList[current].end(); ++it) {
            if (backup_distance[it->first] > it->second) {
              active_vertices.erase({backup_distance[it->first], it->first});
              active_vertices.insert({it->second, it->first});
              backup_distance[it->first] = it->second;
              backup_origin[it->first] = current;
            }
          }
          current = origin[current];
        }
        dijkstra(adjList, backup_distance, backup_origin, active_vertices);
        far_dist = 0;
        for (vector<Vertex>::const_iterator it = terminals.begin();
             it != terminals.end(); ++it) {
          if (far_dist < backup_distance[*it]) {
            far_dist = backup_distance[*it];
            far = *it;
          }
        }
      } else {
        dijk_simple = true;
        current_terminalsMap[far] = current_terminals.size();
        current_terminals.push_back(far);
        steiner_3(adjList, min_distance, origin, root, far, last_far);

        // Update graph
        Vertex current = far;
        while (origin[current] != -1) {
          adjList[current][origin[current]] = 0;
          adjList[origin[current]][current] = 0;
          min_distance[current] = MAX_WEIGHT;
          current = origin[current];
        }
        current = last_far;
        while (origin[current] != -1) {
          adjList[current][origin[current]] = 0;
          adjList[origin[current]][current] = 0;
          min_distance[current] = MAX_WEIGHT;
          current = origin[current];
        }
        dijkstra(adjList, min_distance, origin, root);
        far_dist = 0;
        for (vector<Vertex>::const_iterator it = terminals.begin();
             it != terminals.end(); ++it) {
          if (far_dist < min_distance[*it]) {
            far_dist = min_distance[*it];
            far = *it;
          }
        }
      }
    }

    Tree T(G, root);
    for (unsigned int i = 0; i < G.adjList.size(); ++i) {
      if (origin[i] != -1) {
        T.tree[i].parent = origin[i];
        T.tree[i].weight = G.adjList[i].find(origin[i])->second;
        T.tree[origin[i]].children.insert(i);
      }
    }
    Weight oldw, w = T.pruneLeaves(G.terminalsMap);
    do {
      if (tle)
        break;
      oldw = w;
      apply_opt(T, G);
      w = T.pruneLeaves(G.terminalsMap);
    } while (oldw != w);

    if (tle)
      break;
    adjList = G.adjList;
    origin = full_backup_origin;
    min_distance = full_backup_distance;
    stack<Vertex> next;
    next.push(T.root);
    while (!next.empty()) {
      Vertex current = next.top();
      next.pop();
      for (set<Vertex>::iterator it = T.tree[current].children.begin();
           it != T.tree[current].children.end(); ++it) {
        next.push(*it);
        min_distance[*it] = MAX_WEIGHT;
        origin[*it] = -1;
        adjList[current][*it] = 0;
        adjList[*it][current] = 0;
      }
    }
    if (tle)
      break;
    dijkstra(adjList, min_distance, origin, root);
    far_dist = 0;
    for (vector<Vertex>::const_iterator it = terminals.begin();
         it != terminals.end(); ++it) {
      if (far_dist < min_distance[*it]) {
        far_dist = min_distance[*it];
        far = *it;
      }
    }
  }

  // build tree
  Tree T(G, root);
  for (unsigned int i = 0; i < G.adjList.size(); ++i) {
    if (origin[i] != -1) {
      T.tree[i].parent = origin[i];
      T.tree[i].weight = G.adjList[i].find(origin[i])->second;
      T.tree[origin[i]].children.insert(i);
    }
  }
  return T;
}
bool cmp(const tuple<Weight, Vertex, Vertex> &x,
         const tuple<Weight, Vertex, Vertex> &y) {
  return get<0>(x) > get<0>(y);
}

Tree mst(const Graph &G, Vertex root) {
  Tree T(G, root);
  vector<tuple<Weight, Vertex, Vertex>> active_vertices;
  for (pair<Vertex, Weight> neighbour : G.adjList[root]) {
    active_vertices.push_back({neighbour.second, root, neighbour.first});
  }
  make_heap(active_vertices.begin(), active_vertices.end(), cmp);
  while (!active_vertices.empty()) {
    std::pop_heap(active_vertices.begin(), active_vertices.end(), cmp);
    tuple<Weight, Vertex, Vertex> edge = active_vertices.back();
    active_vertices.pop_back();
    if (T.tree[get<2>(edge)].parent == -2) {
      T.tree[get<2>(edge)].parent = get<1>(edge);
      T.tree[get<2>(edge)].weight = get<0>(edge);
      T.tree[get<1>(edge)].children.insert(get<2>(edge));
      for (pair<Vertex, Weight> neighbour : G.adjList[get<2>(edge)]) {
        if (T.tree[neighbour.first].parent == -2) {
          active_vertices.push_back(
              {neighbour.second, get<2>(edge), neighbour.first});
          push_heap(active_vertices.begin(), active_vertices.end(), cmp);
        }
      }
    }
  }
  return T;
}

Tree random(const Graph &G, Vertex root) {
  Tree T(G, root);
  default_random_engine generator;
  vector<tuple<Weight, Vertex, Vertex>> active_vertices;
  for (pair<Vertex, Weight> neighbour : G.adjList[root]) {
    active_vertices.push_back({neighbour.second, root, neighbour.first});
  }
  while (!active_vertices.empty()) {
    int position =
        uniform_int_distribution<int>(0, active_vertices.size() - 1)(generator);
    tuple<Weight, Vertex, Vertex> edge = active_vertices[position];
    active_vertices[position] = active_vertices.back();
    active_vertices.pop_back();
    if (T.tree[get<2>(edge)].parent == -2) {
      T.tree[get<2>(edge)].parent = get<1>(edge);
      T.tree[get<2>(edge)].weight = get<0>(edge);
      T.tree[get<1>(edge)].children.insert(get<2>(edge));
      for (pair<Vertex, Weight> neighbour : G.adjList[get<2>(edge)]) {
        if (T.tree[neighbour.first].parent == -2) {
          active_vertices.push_back(
              {neighbour.second, get<2>(edge), neighbour.first});
          push_heap(active_vertices.begin(), active_vertices.end(), cmp);
        }
      }
    }
  }
  return T;
}
