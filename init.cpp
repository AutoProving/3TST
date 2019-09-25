#include "init.h"

Tree spanningTree(Graph &G, Vertex root) {
  if (root < 0 || root > G.numberVertices)
    throw "spanningTree";
  Tree T(G, root);
  set<pair<Weight, pair<Vertex, Vertex>>> next;
  map<Vertex, shared_ptr<treeNode>> nodes;
  nodes.insert(pair<Vertex, shared_ptr<treeNode>>(root, T.root));
  for (map<Vertex, Weight>::const_iterator it = G.adjList[root].begin();
       it != G.adjList[root].end(); ++it) {
    next.insert({it->second, {root, it->first}});
  }
  while (!next.empty()) {
    pair<Vertex, Vertex> current = next.begin()->second;
    Weight w = next.begin()->first;
    next.erase(next.begin());
    if (nodes.find(current.second) == nodes.end()) {
      shared_ptr<treeNode> parent = nodes.find(current.first)->second;
      shared_ptr<treeNode> N = parent->addChild(current.second, w);
      nodes.insert(pair<Vertex, shared_ptr<treeNode>>(current.first, N));
      for (map<Vertex, Weight>::const_iterator it =
               G.adjList[current.second].begin();
           it != G.adjList[current.second].end(); ++it) {
        if (nodes.find(it->first) == nodes.end())
          next.insert({it->second, {current.second, it->first}});
      }
    }
  }
  return T;
}

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
  // Merge origin1 into origin0
  Vertex current = intersect;
  while (origin1[current] != -1) {
    if (origin1[current] != origin0[current]) {
      origin0[origin1[current]] = current;
    }
    current = origin1[current];
  }
  // Merge origin2 into origin0
  current = intersect;
  while (origin2[current] != -1) {
    if (origin2[current] != origin0[current]) {
      origin0[origin2[current]] = current;
    }
    current = origin2[current];
  }
}

//TODO This can be simply by the algo from the paper. Try to rewrite it
Tree incrementalDijks3(const Graph &G, Vertex root,
                       const vector<int> &terminalsMap,
                       const vector<Vertex> &terminals) {
  vector<std::map<Vertex, Weight>> adjList = G.adjList;
  vector<Weight> min_distance(G.numberVertices, MAX_WEIGHT);
  vector<Vertex> origin(G.numberVertices, -1);
  Vertex far = dijkstra(adjList, min_distance, origin, root, terminalsMap,
                        terminals.size());
  Weight far_dist = min_distance[far];

  Vertex last_far = far;
  bool dijk_simple = true;
  while (far_dist > 0 && !tle) {
    if (dijk_simple) {
      dijk_simple = false;
      vector<Weight> backup_distance = min_distance;
      vector<Vertex> backup_origin = origin;

      // Update graph, set edge to 0
      last_far = far; //TODO: virer, inutile, il faut juste un valeur far pour chaque cas du if, pas besoin de copier
      Vertex current = far;
      while (origin[current] != -1 && backup_distance[current] != 0) {
        backup_distance[current] = 0;
        current = origin[current];
      }
      current = far;
      set<pair<Weight, Vertex>> active_vertices;
      while (origin[current] != -1 && min_distance[current] != 0) {
        for (map<Vertex, Weight>::const_iterator it = adjList[current].begin();
             it != adjList[current].end(); ++it) {
          if (it->first != backup_origin[current] &&
              backup_distance[it->first] > it->second) {
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
      far = dijkstra(adjList, min_distance, origin, root, terminalsMap,
                     terminals.size()); //TODO Change to simple dijkstra, quand les nœuds sont dans l'arbre courant ils ne peuvent pas être attiends c'est plus lent pour rien. Soit mettre le simple ou faire un truc un peu plus intelligent mais ça ne vaut pas le coup en terme de gain de temps.
      if (far == -1) {
        far_dist = 0;
        for (vector<Vertex>::const_iterator it = terminals.begin();
             it != terminals.end(); ++it) {
          if (far_dist < min_distance[*it]) {
            far_dist = min_distance[*it];
            far = *it;
          }
        }
      } else {
        far_dist = min_distance[far];
      }
    }
  }

  // build tree
  Tree T(G, root, terminalsMap, terminals);
  vector<shared_ptr<treeNode>> nodes;
  for (int i = 0; i < G.numberVertices; ++i) {
    nodes.push_back(shared_ptr<treeNode>(new treeNode(i)));
  }
  nodes[root] = T.root;
  for (int i = 0; i < G.numberVertices; ++i) {
    if (origin[i] != -1) {
      nodes[i]->parent = nodes[origin[i]].get();
      nodes[i]->w = G.adjList[i].find(origin[i])->second;
      nodes[origin[i]]->addChild(nodes[i]);
    }
  }
  return T;
}

Tree incrementalOptDijks3(const Graph &G, Vertex root,
                          const vector<int> &terminalsMap,
                          const vector<Vertex> &terminals) {
  vector<std::map<Vertex, Weight>> adjList = G.adjList;
  vector<Weight> min_distance(G.numberVertices, MAX_WEIGHT);
  vector<Vertex> origin(G.numberVertices, -1);
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
        far = dijkstra(adjList, min_distance, origin, root, terminalsMap,
                       terminals.size());
        if (far == -1) {
          far_dist = 0;
          for (vector<Vertex>::const_iterator it = terminals.begin();
               it != terminals.end(); ++it) {
            if (far_dist < min_distance[*it]) {
              far_dist = min_distance[*it];
              far = *it;
            }
          }
        } else {
          far_dist = min_distance[far];
        }
      }
    }

    Tree T(G, root, current_terminalsMap, current_terminals);
    vector<shared_ptr<treeNode>> nodes;
    nodes.reserve(G.numberVertices);
    for (int i = 0; i < G.numberVertices; ++i) {
      nodes.push_back(shared_ptr<treeNode>(new treeNode(i)));
    }
    nodes[root] = T.root;
    for (int i = 0; i < G.numberVertices; ++i) {
      if (origin[i] != -1) {
        nodes[i]->parent = nodes[origin[i]].get();
        nodes[i]->w = G.adjList[i].find(origin[i])->second;
        nodes[origin[i]]->addChild(nodes[i]);
      }
    }
    Weight oldw, w = T.pruneLeaves();
    do {
      if (tle)
        break;
      oldw = w;
      apply_opt(T);
      w = T.pruneLeaves();
    } while (oldw != w);

    if (tle)
      break;
    adjList = G.adjList;
    origin = full_backup_origin;
    min_distance = full_backup_distance;
    stack<shared_ptr<treeNode>> next;
    next.push(T.root);
    while (!next.empty()) {
      shared_ptr<treeNode> current = next.top();
      next.pop();
      for (map<Vertex, shared_ptr<treeNode>>::iterator it =
               current->children.begin();
           it != current->children.end(); ++it) {
        next.push(it->second);
        min_distance[it->first] = MAX_WEIGHT;
        origin[it->first] = -1;
        adjList[current->v][it->first] = 0;
        adjList[it->first][current->v] = 0;
      }
    }
    if (tle)
      break;
    far = dijkstra(adjList, min_distance, origin, root, terminalsMap,
                   terminals.size());
    if (far == -1) {
      far_dist = 0;
      for (vector<Vertex>::const_iterator it = terminals.begin();
           it != terminals.end(); ++it) {
        if (far_dist < min_distance[*it]) {
          far_dist = min_distance[*it];
          far = *it;
        }
      }
    } else {
      far_dist = min_distance[far];
    }
  }

  // build tree
  Tree T(G, root, terminalsMap, terminals);
  vector<shared_ptr<treeNode>> nodes;
  nodes.reserve(G.numberVertices);
  for (int i = 0; i < G.numberVertices; ++i) {
    nodes.push_back(shared_ptr<treeNode>(new treeNode(i)));
  }
  nodes[root] = T.root;
  for (int i = 0; i < G.numberVertices; ++i) {
    if (origin[i] != -1) {
      nodes[i]->parent = nodes[origin[i]].get();
      nodes[i]->w = G.adjList[i].find(origin[i])->second;
      nodes[origin[i]]->addChild(nodes[i]);
    }
  }
  return T;
}
