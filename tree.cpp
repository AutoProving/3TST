#include "tree.h"

using namespace std;

shared_ptr<treeNode> treeNode::addChild(Vertex v, Weight w) {
  shared_ptr<treeNode> child(new treeNode(v, w, this));
  children.insert({v, child});
  return child;
}

shared_ptr<treeNode> treeNode::addChild(shared_ptr<treeNode> N) {
  children.insert(pair<Vertex, shared_ptr<treeNode>>(N->v, N));
  return N;
}

void treeNode::print() {
  cout << "Node: " << v;
  if (parent)
    cout << " ,parent: " << parent->v << " weight: " << w;
  cout << endl;
}

void treeNode::printSubTree() {
  stack<treeNode> next;
  next.push(*this);
  while (!next.empty()) {
    treeNode current = next.top();
    next.pop();
    for (map<Vertex, shared_ptr<treeNode>>::iterator it =
             current.children.begin();
         it != current.children.end(); ++it) {
      cout << current.v + 1 << " " << it->first + 1 << endl;
      next.push(*it->second);
    }
  }
}

int treeNode::pruneLeaves(const vector<int> &terminalsMap) {
  int tmp = 0;
  map<Vertex, shared_ptr<treeNode>> backup = children;
  for (map<Vertex, shared_ptr<treeNode>>::iterator it = backup.begin();
       it != backup.end(); ++it) {
    tmp += it->second->pruneLeaves(terminalsMap);
  }
  if (children.empty() && terminalsMap[v] == -1) {
    parent->children.erase(v);
  } else {
    if (parent)
      tmp += w;
  }
  return tmp;
}

void Tree::print() { root->printSubTree(); }

void Tree::print(const map<pair<Vertex, Vertex>, vector<Vertex>> &hash) {
  stack<shared_ptr<treeNode>> next;
  next.push(root);
  while (!next.empty()) {
    shared_ptr<treeNode> current = next.top();
    next.pop();
    for (map<Vertex, shared_ptr<treeNode>>::iterator it =
             current->children.begin();
         it != current->children.end(); ++it) {
      cout << current->v + 1 << " ";
      map<pair<Vertex, Vertex>, vector<Vertex>>::const_iterator it_path =
          hash.find({current->v, it->first});
      if (it_path != hash.end()) {
        for (vector<Vertex>::const_iterator path = it_path->second.begin();
             path != it_path->second.end(); ++path) {
          cout << *path + 1 << endl << *path + 1 << " ";
        }
      }
      cout << it->first + 1 << endl;
      next.push(it->second);
    }
  }
}

void Tree::pruneRoot() {
  if (root->children.size() == 1 && terminalsMap[root->v] == -1) {
    root = root->children.begin()->second;
    root->parent = NULL;
    root->w = MAX_WEIGHT;
    pruneRoot();
  }
}

Weight Tree::pruneLeaves() {
  Weight tmp = 0;
  stack<shared_ptr<treeNode>> next;
  vector<shared_ptr<treeNode>> to_clean;
  next.push(root);
  while (!next.empty()) {
    shared_ptr<treeNode> current = next.top();
    next.pop();
    for (map<Vertex, shared_ptr<treeNode>>::iterator it =
             current->children.begin();
         it != current->children.end(); ++it) {
      if (it->second->children.size() == 0 && terminalsMap[it->first] == -1)
        to_clean.push_back(it->second);
      next.push(it->second);
      tmp += it->second->w;
    }
  }

  for (vector<shared_ptr<treeNode>>::iterator it = to_clean.begin();
       it != to_clean.end(); ++it) {
    treeNode *current = it->get();
    while (current->children.size() == 0 && terminalsMap[current->v] == -1 &&
           current->parent != NULL) {
      tmp -= current->w;
      treeNode *next = current->parent;
      current->parent->children.erase(current->v);
      current = next;
    }
  }
  return tmp;
}

int Tree::size() {
  int tmp = 0;
  stack<shared_ptr<treeNode>> next;
  next.push(root);
  while (!next.empty()) {
    shared_ptr<treeNode> current = next.top();
    next.pop();
    for (map<Vertex, shared_ptr<treeNode>>::iterator it =
             current->children.begin();
         it != current->children.end(); ++it) {
      next.push(it->second);
      tmp += it->second->w;
    }
  }

  return tmp;
}

Tree::Tree(const Graph &G, istream &input)
    : G{G}, terminalsMap{G.terminalsMap}, terminals{G.terminals} {
  std::string line;
  while (line.size() == 0 || line[0] != 'V')
    getline(input, line);

  vector<vector<Vertex>> tree(G.numberVertices, vector<Vertex>(0,-1));
  while (getline(input, line)) {
    Vertex v1, v2;
    std::stringstream(line) >> v1 >> v2;
    v1--;
    v2--;
    tree[v1].push_back(v2);
    tree[v2].push_back(v1);
  }

  vector<shared_ptr<treeNode>> nodes;
  for (int i = 0; i < G.numberVertices; ++i) {
    nodes.push_back(shared_ptr<treeNode>(new treeNode(i)));
  }
  vector<Vertex> dfs;
  dfs.reserve(G.numberVertices);
  dfs.push_back(G.terminals[0]);
  root = nodes[G.terminals[0]];
  for (int i = 0; i < G.numberVertices; ++i) {
      Vertex u = dfs[i];
      for(auto& v:tree[u]){
          if (nodes[u]->parent == NULL || nodes[u]->parent->v != v){
              nodes[v]->parent = nodes[u].get();
              nodes[v]->w = G.adjList[v].find(u)->second;
              nodes[u]->addChild(nodes[v]);
              dfs.push_back(v);
          }
      }
  }
}
