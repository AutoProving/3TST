#include "opt.h"

volatile sig_atomic_t tle = false;

void signalHandler(int) { tle = true; }

inline void initPriorityQueue(const shared_ptr<treeNode> &node,
                              const vector<map<Vertex, Weight>> &adjList,
                              vector<Weight> &min_distance,
                              vector<Vertex> &origin,
                              set<pair<Weight, Vertex>> &active_vertices) {
  queue<shared_ptr<treeNode>> next;
  next.push(node);
  while (!next.empty()) {
    shared_ptr<treeNode> current = next.front();
    next.pop();

    for (map<Vertex, Weight>::const_iterator it = adjList[current->v].begin();
         it != adjList[current->v].end(); ++it) {
      map<Vertex, shared_ptr<treeNode>>::iterator son =
          current->children.find(it->first);
      if (son != current->children.end()) {
        next.push(son->second);
        active_vertices.erase({min_distance[it->first], it->first});
        min_distance[it->first] = 0;
        origin[it->first] = current->v;
      } else if (min_distance[it->first] > it->second) {
        active_vertices.erase({min_distance[it->first], it->first});
        active_vertices.insert({it->second, it->first});
        min_distance[it->first] = it->second;
        origin[it->first] = current->v;
      }
    }
  }
}

inline treeNode *deleteUpPath(treeNode *tmp, const vector<int> &terminalsMap,
                              vector<bool> &inTree) {
  while (tmp->children.empty() && terminalsMap[tmp->v] == -1 &&
         tmp->parent != NULL) {
    Vertex v = tmp->v;
    inTree[v] = false;
    tmp = tmp->parent;
    tmp->children.erase(v);
  }
  return tmp;
}

inline void buildDownPath(const vector<map<Vertex, Weight>> &adjList,
                          const vector<Weight> &min_distance,
                          const vector<Vertex> &origin, Vertex cible,
                          vector<bool> &inTree,
                          vector<weak_ptr<treeNode>> &mapNode) {
  shared_ptr<treeNode> current = mapNode[cible].lock();
  while (origin[current->v] != -1) {
    if (!inTree[origin[current->v]]) {
      inTree[origin[current->v]] = true;
      current = current->addChild(origin[current->v],
                                  min_distance[current->v] -
                                      min_distance[origin[current->v]]);
      mapNode[current->v] = current;
    } else {
      shared_ptr<treeNode> son = mapNode[origin[current->v]].lock();
      if (son->parent != NULL) {
        son->parent->children.erase(son->v);
      }
      son->parent = current.get();
      son->w = adjList[son->v].find(current->v)->second;
      current = current->addChild(son);
    }
  }
}

inline void buildUpPath(shared_ptr<treeNode> current,
                        const vector<map<Vertex, Weight>> &adjList,
                        const vector<Weight> &min_distance,
                        const vector<Vertex> &origin, vector<bool> &inTree,
                        vector<weak_ptr<treeNode>> &mapNode) {
  while (origin[current->v] != -1) {
    inTree[current->v] = true;
    if (current->parent != NULL)
      current->parent->children.erase(current->v);
    if (!inTree[origin[current->v]]) {
      shared_ptr<treeNode> tmp(new treeNode(origin[current->v]));
      mapNode[tmp->v] = tmp;
      tmp->addChild(current);
      current->parent = tmp.get();
      current->w =
          min_distance[current->v] - min_distance[origin[current->v]]; // vrai ?
      current = tmp;
    } else {
      shared_ptr<treeNode> tmp = mapNode[origin[current->v]].lock();
      tmp->addChild(current);
      // if (current->parent != NULL && current->parent->v ==
      // origin[current->v]) break;
      current->parent = tmp.get();
      current->w = adjList[current->v].find(tmp->v)->second;
      current = tmp;
    }
  }
}

inline void opt_d3(const shared_ptr<treeNode> &root, shared_ptr<treeNode> &v1,
                   shared_ptr<treeNode> &v2, const int numberVertices,
                   const vector<map<Vertex, Weight>> &adjList,
                   vector<Vertex> terminalsMap,
                   vector<weak_ptr<treeNode>> &mapNode, vector<bool> &inTree) {

  if (v1->parent != NULL) {
    v1->parent->children.erase(v1->v);
    deleteUpPath(v1->parent, terminalsMap, inTree);
  }
  if (v2->parent != NULL) {
    v2->parent->children.erase(v2->v);
    deleteUpPath(v2->parent, terminalsMap, inTree);
  }

  while (v1->children.size() == 1 && terminalsMap[v1->v] == -1) {
    inTree[v1->v] = false;
    v1 = v1->children.begin()->second;
  }
  v1->parent = NULL;
  v1->w = MAX_WEIGHT;

  while (v2->children.size() == 1 && terminalsMap[v2->v] == -1) {
    inTree[v2->v] = false;
    v2 = v2->children.begin()->second;
  }
  v2->parent = NULL;
  v2->w = MAX_WEIGHT;

  // Dijkstra from v1
  set<pair<Weight, Vertex>> active_vertices;
  vector<Weight> min_distance1(numberVertices, MAX_WEIGHT);
  vector<Vertex> origin1(numberVertices, -1);
  min_distance1[v1->v] = 0;

  initPriorityQueue(v1, adjList, min_distance1, origin1, active_vertices);
  Vertex c1 =
      dijkstra(adjList, min_distance1, origin1, inTree, active_vertices);

  // Dijkstra from v2
  active_vertices.clear();
  vector<Weight> min_distance2(numberVertices, MAX_WEIGHT);
  vector<Vertex> origin2(numberVertices, -1);
  min_distance2[v2->v] = 0;

  initPriorityQueue(v2, adjList, min_distance2, origin2, active_vertices);
  Vertex c2 =
      dijkstra(adjList, min_distance2, origin2, inTree, active_vertices);

  // Dijkstra from root
  active_vertices.clear();
  vector<Weight> min_distance0(numberVertices, MAX_WEIGHT);
  vector<Vertex> origin0(numberVertices, -1);
  min_distance0[root->v] = 0;

  initPriorityQueue(root, adjList, min_distance0, origin0, active_vertices);
  Vertex c0 =
      dijkstra(adjList, min_distance0, origin0, inTree, active_vertices);

  Vertex r1;
  if (min_distance2[c1] == 0) {
    r1 = v2->v;
  } else {
    r1 = root->v;
  }
  Vertex r2;
  if (min_distance1[c2] == 0) {
    r2 = v1->v;
  } else {
    r2 = root->v;
  }
  Vertex r0;
  if (min_distance1[c0] == 0) {
    r0 = v1->v;
  } else {
    r0 = v2->v;
  }

  Vertex intersect = -1;
  Weight distIntersect = MAX_WEIGHT;
  if (r1 == root->v || r2 == root->v) {
    distIntersect = min_distance1[c1] + min_distance2[c2];
  } else if (r0 == v1->v) {
    distIntersect = min_distance0[c0] + min_distance2[c2];
  } else {
    distIntersect = min_distance0[c0] + min_distance1[c1];
  }
  for (Vertex i = 0; i < numberVertices; ++i) {
    if (min_distance2[i] != MAX_WEIGHT && min_distance1[i] != MAX_WEIGHT &&
        min_distance0[i] != MAX_WEIGHT &&
        min_distance0[i] + min_distance1[i] + min_distance2[i] <
            distIntersect) {
      intersect = i;
      distIntersect = min_distance0[i] + min_distance1[i] + min_distance2[i];
    }
  }

  if (intersect != -1) {
    shared_ptr<treeNode> current;
    if (!inTree[intersect]) {
      current = shared_ptr<treeNode>(new treeNode(intersect));
      mapNode[intersect] = current;
    } else {
      current = mapNode[intersect].lock();
    }
    // Build the tree with origin1
    buildDownPath(adjList, min_distance1, origin1, intersect, inTree, mapNode);

    // Build the tree with origin2
    buildDownPath(adjList, min_distance2, origin2, intersect, inTree, mapNode);

    // Build the tree with origin0
    buildUpPath(current, adjList, min_distance0, origin0, inTree, mapNode);
  } else {
    if (r2 == root->v) {
      // Build the tree with origin1
      buildDownPath(adjList, min_distance1, origin1, c1, inTree, mapNode);
      // Build the tree with origin2
      buildDownPath(adjList, min_distance2, origin2, c2, inTree, mapNode);
    } else if (r1 == root->v) {
      // Build the tree with origin2
      buildDownPath(adjList, min_distance2, origin2, c2, inTree, mapNode);
      // Build the tree with origin1
      buildDownPath(adjList, min_distance1, origin1, c1, inTree, mapNode);
    } else {
      shared_ptr<treeNode> current = mapNode[c0].lock();

      // Build the tree with origin1
      if (r0 == v1->v) {
        // Build the tree with origin2
        buildDownPath(adjList, min_distance2, origin2, c2, inTree, mapNode);
        // Build the tree with origin1
        buildDownPath(adjList, min_distance1, origin1, c0, inTree, mapNode);
      } else {
        // Build the tree with origin1
        buildDownPath(adjList, min_distance1, origin1, c1, inTree, mapNode);
        // Build the tree with origin2
        buildDownPath(adjList, min_distance2, origin2, c0, inTree, mapNode);
      }

      // Build the tree with origin0
      buildUpPath(current, adjList, min_distance0, origin0, inTree, mapNode);
    }
  }
}

inline void opt_d3_parent(shared_ptr<treeNode> &node,
                          const shared_ptr<treeNode> &root,
                          const int numberVertices,
                          const vector<map<Vertex, Weight>> &adjList,
                          const vector<int> &terminalsMap,
                          vector<weak_ptr<treeNode>> &mapNode,
                          vector<bool> &inTree) {
  vector<pair<Vertex, weak_ptr<treeNode>>> backup_children(
      node->children.begin(), node->children.end());
  for (vector<pair<Vertex, weak_ptr<treeNode>>>::iterator it1 =
           backup_children.begin();
       it1 != backup_children.end(); ++it1) {
    if (!node || node->parent == NULL || tle)
      return;
    map<Vertex, shared_ptr<treeNode>>::iterator tmp_it =
        node->children.find(it1->first);
    if (tmp_it == node->children.end())
      break;
    shared_ptr<treeNode> v1 = tmp_it->second;
    shared_ptr<treeNode> v2 = node;

    node->parent->children.erase(node->v);
    deleteUpPath(node->parent, terminalsMap, inTree);

    // delete path starting with it1 to the first uselfull son
    node->children.erase(v1->v);
    while (v1->children.size() == 1 && terminalsMap[v1->v] == -1) {
      inTree[v1->v] = false;
      v1 = v1->children.begin()->second;
    }
    v1->parent = NULL;
    v1->w = MAX_WEIGHT;

    // delete path starting with it2 to the first uselfull son
    while (v2->children.size() == 1 && terminalsMap[v2->v] == -1) {
      inTree[v2->v] = false;
      v2 = v2->children.begin()->second;
    }
    v2->parent = NULL;
    v2->w = MAX_WEIGHT;

    if (node->v != v2->v)
      node.reset();

    opt_d3(root, v1, v2, numberVertices, adjList, terminalsMap, mapNode,
           inTree);
  }
}

inline void
opt_d3_son(shared_ptr<treeNode> &node, const shared_ptr<treeNode> &root,
           const int numberVertices, const vector<map<Vertex, Weight>> &adjList,
           const vector<int> &terminalsMap, vector<weak_ptr<treeNode>> &mapNode,
           vector<bool> &inTree) {
  vector<pair<Vertex, weak_ptr<treeNode>>> backup_children(
      node->children.begin(), node->children.end());
  for (vector<pair<Vertex, weak_ptr<treeNode>>>::iterator it1 =
           backup_children.begin();
       it1 != backup_children.end(); ++it1) {
    if (!node)
      break;
    for (vector<pair<Vertex, weak_ptr<treeNode>>>::iterator it2 = it1 + 1;
         it2 != backup_children.end(); ++it2) {
      if (tle)
        return;
      if (!node)
        break;
      map<Vertex, shared_ptr<treeNode>>::iterator tmp_it =
          node->children.find(it1->first);
      if (tmp_it == node->children.end())
        break;
      shared_ptr<treeNode> v1 = tmp_it->second;
      tmp_it = node->children.find(it2->first);
      if (tmp_it == node->children.end())
        continue;
      shared_ptr<treeNode> v2 = tmp_it->second;

      opt_d3(root, v1, v2, numberVertices, adjList, terminalsMap, mapNode,
             inTree);
    }
  }
}

void apply_opt(Tree &T) {
  vector<weak_ptr<treeNode>> mapNode(T.G.numberVertices);
  vector<bool> inTree(T.G.numberVertices, false);
  stack<Vertex> next_opt;
  {
    queue<shared_ptr<treeNode>> next;
    next.push(T.root);

    // We start the optimisation by the leaves
    // Fill next_opt with the order
    while (!next.empty()) {
      shared_ptr<treeNode> current = next.front();
      next.pop();

      mapNode[current->v] = current;
      inTree[current->v] = true;

      for (map<Vertex, shared_ptr<treeNode>>::iterator it =
               current->children.begin();
           it != current->children.end(); ++it) {
        next.push(it->second);
        if (tle)
          return;
      }
      next_opt.push(current->v);
    }
  }

  // apply optimisation following the next_opt order
  while (!next_opt.empty()) {
    weak_ptr<treeNode> current = mapNode[next_opt.top()];
    next_opt.pop();
    if (!current.expired()) {
      shared_ptr<treeNode> node = current.lock();

      // Optimisation for node of degree 3 or more
      if (tle)
        return;
      if (node->children.size() > 1) {
        opt_d3_son(node, T.root, T.G.numberVertices, T.G.adjList,
                   T.terminalsMap, mapNode, inTree);
      }
      if (tle)
        return;
      if (node->children.size() > 1 || T.terminalsMap[node->v] != -1) {
        opt_d3_parent(node, T.root, T.G.numberVertices, T.G.adjList,
                      T.terminalsMap, mapNode, inTree);
      }
    }
  }
}

void full_d3(Tree &T) {
  vector<weak_ptr<treeNode>> mapNode(T.G.numberVertices);
  vector<bool> inTree(T.G.numberVertices, false);
  vector<Vertex> next_opt;
  next_opt.reserve(T.G.numberVertices);
  {
    queue<shared_ptr<treeNode>> next;
    next.push(T.root);

    while (!next.empty()) {
      shared_ptr<treeNode> current = next.front();
      next.pop();

      mapNode[current->v] = current;
      inTree[current->v] = true;

      for (map<Vertex, shared_ptr<treeNode>>::iterator it =
               current->children.begin();
           it != current->children.end(); ++it) {
        if (it->second->children.size() > 1 || T.terminalsMap[it->first])
          next_opt.push_back(it->first);
        next.push(it->second);
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
      if (inTree[*it1]) {
        if (inTree[*it2]) {
          shared_ptr<treeNode> v1 = mapNode[*it1].lock();
          shared_ptr<treeNode> v2 = mapNode[*it2].lock();
          if ((v1->children.size() > 1 || T.terminalsMap[*it1] != -1) &&
              (v2->children.size() > 1 || T.terminalsMap[*it2] != -1)) {
            opt_d3(T.root, v1, v2, T.G.numberVertices, T.G.adjList,
                   T.terminalsMap, mapNode, inTree);
          }
        }
      }
    }
  }
}

pair<Tree, Weight> complet_heuristic(const Graph &G,
                                     const vector<int> &terminalsMap,
                                     const vector<Vertex> &terminals) {
  switch (terminals.size()) {
  case 0:
    return {Tree(G, terminalsMap, terminals), 0};
  case 1:
    return {Tree(G, terminals[0], terminalsMap, terminals), 0};
  case 2: {
    Tree T = incrementalDijks3(G, terminals[0], terminalsMap, terminals);
    return {T, T.pruneLeaves()};
  }
  case 3: {
    Tree T = incrementalDijks3(G, terminals[0], terminalsMap, terminals);
    return {T, T.pruneLeaves()};
  }
  default: {
    Tree T(G);
    Weight w, oldw, wf = MAX_WEIGHT;
    for (vector<Vertex>::const_iterator it = terminals.begin();
         it != terminals.end(); ++it) {
      if (tle)
        break;
      Tree tmp = incrementalDijks3(G, *it, terminalsMap, terminals);
      w = tmp.pruneLeaves();
      do {
        if (tle)
          break;
        oldw = w;
        apply_opt(tmp);
        w = tmp.pruneLeaves();
      } while (oldw != w);
      do {
        if (tle)
          break;
        oldw = w;
        full_d3(tmp);
        w = tmp.pruneLeaves();
      } while (oldw != w);
      if (wf > w) {
        wf = w;
        T.root = tmp.root;
      }
    }
    for (vector<Vertex>::const_iterator it = terminals.begin();
         it != terminals.end(); ++it) {
      if (tle)
        break;
      Tree tmp = incrementalOptDijks3(G, *it, terminalsMap, terminals);
      w = tmp.pruneLeaves();
      do {
        if (tle)
          break;
        oldw = w;
        apply_opt(tmp);
        w = tmp.pruneLeaves();
      } while (oldw != w);
      do {
        if (tle)
          break;
        oldw = w;
        full_d3(tmp);
        w = tmp.pruneLeaves();
      } while (oldw != w);
      if (wf > w) {
        wf = w;
        T.root = tmp.root;
      }
    }
    return {T, wf};
  }
  }
}
