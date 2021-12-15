#include "tree.hpp"

using namespace std;

void treeNode::remove(Vertex v) { children.erase(v); }

bool Tree::check(const Graph &G) {
  bool b = true;
  int r = 0;
  for (unsigned int i = 0; i < tree.size(); ++i) {
    if (tree[i].parent >= 0) {
      if (tree[tree[i].parent].children.find(i) ==
          tree[tree[i].parent].children.end()) {
        cout << "parent " << i << " " << tree[i].parent << endl;
        throw 20;
      }
      auto it = G.adjList[tree[i].parent].find(i);
      if (it == G.adjList[tree[i].parent].end()) {
        cout << "graph" << endl;
        throw 10;
      } else {
        if (tree[i].weight != it->second) {
          cout << "weight " << tree[i].weight << " " << it->second << endl;
          throw 10;
        }
      }
    } else {
      if (tree[i].parent == -1) {
        if (++r > 1) {
          cout << "Too many root" << endl;
          throw 30;
        }
      } else {
        if (tree[i].parent != -2) {
          cout << "unknown value " << tree[i].parent << endl;
          throw 50;
        }
      }
    }
    for (auto c : tree[i].children) {
      if ((unsigned int)tree[c].parent != i) {
        cout << "child " << c << " " << i;
        throw 40;
      }
    }
  }
  return b;
}

void Tree::print() {
  stack<Vertex> next;
  next.push(root);
  while (!next.empty()) {
    Vertex current = next.top();
    next.pop();
    for (set<Vertex>::iterator it = tree[current].children.begin();
         it != tree[current].children.end(); ++it) {
      cout << current + 1 << " " << *it + 1 << endl;
      next.push(*it);
    }
  }
  cout.flush();
}

void Tree::print(const map<pair<Vertex, Vertex>, vector<Vertex>> &hash) {
  stack<Vertex> next;
  next.push(root);
  while (!next.empty()) {
    Vertex current = next.top();
    next.pop();
    for (set<Vertex>::iterator it = tree[current].children.begin();
         it != tree[current].children.end(); ++it) {
      cout << current + 1 << " ";
      map<pair<Vertex, Vertex>, vector<Vertex>>::const_iterator it_path =
          hash.find({current, *it});
      if (it_path != hash.end()) {
        for (vector<Vertex>::const_iterator path = it_path->second.begin();
             path != it_path->second.end(); ++path) {
          cout << *path + 1 << endl << *path + 1 << " ";
        }
      }
      cout << *it + 1 << endl;
      next.push(*it);
    }
  }
  cout.flush();
}

void Tree::pruneRoot(const vector<int> &terminalsMap) {
  if (tree[root].children.size() == 1 && terminalsMap[root] == -1) {
    Vertex new_root = *(tree[root].children.begin());
    tree[root].parent = -2;
    tree[root].children.clear();
    root = new_root;
    tree[root].parent = -1;
    tree[root].weight = 0;
    pruneRoot(terminalsMap);
  }
}

Weight Tree::pruneLeaves(const vector<int> &terminalsMap) {
  Weight tmp = 0;
  stack<Vertex> next;
  vector<Vertex> to_clean;
  next.push(root);
  while (!next.empty()) {
    Vertex current = next.top();
    next.pop();
    for (set<Vertex>::iterator it = tree[current].children.begin();
         it != tree[current].children.end(); ++it) {
      if (tree[*it].children.size() == 0 && terminalsMap[*it] == -1)
        to_clean.push_back(*it);
      next.push(*it);
      tmp += tree[*it].weight;
    }
  }

  for (vector<Vertex>::iterator it = to_clean.begin(); it != to_clean.end();
       ++it) {
    Vertex current = *it;
    while (tree[current].children.size() == 0 && terminalsMap[current] == -1 &&
           tree[current].parent >= 0) {
      tmp -= tree[current].weight;
      Vertex next = tree[current].parent;
      tree[next].remove(current);
      tree[current].parent = -2;
      current = next;
    }
  }
  return tmp;
}

int Tree::size() {
  int tmp = 0;
  stack<Vertex> next;
  next.push(root);
  while (!next.empty()) {
    Vertex current = next.top();
    next.pop();
    for (set<Vertex>::iterator it = tree[current].children.begin();
         it != tree[current].children.end(); ++it) {
      next.push(*it);
      tmp += tree[*it].weight;
    }
  }

  return tmp;
}

Tree::Tree(const Graph &G, Vertex root) : root{root} {
  tree.resize(G.adjList.size(), treeNode());
  tree[root].parent = -1;
}

Tree::Tree(const Graph &G, istream &input) {
  std::string line;
  while (line.size() == 0 || line[0] != 'V')
    getline(input, line);

  vector<vector<Vertex>> graph(G.adjList.size(), vector<Vertex>(0, -1));
  while (getline(input, line)) {
    Vertex v1, v2;
    std::stringstream(line) >> v1 >> v2;
    v1--;
    v2--;
    graph[v1].push_back(v2);
    graph[v2].push_back(v1);
  }

  tree.resize(G.adjList.size());

  vector<Vertex> dfs;
  dfs.reserve(G.adjList.size());
  root = G.terminals[0];
  dfs.push_back(root);
  tree[root].parent = -1;
  for (unsigned int i = 0; i < G.adjList.size(); ++i) {
    Vertex u = dfs[i];
    for (auto &v : graph[u]) {
      if (tree[v].parent == -2) {
        tree[u].children.insert(v);
        tree[v].parent = u;
        tree[v].weight = G.adjList[v].find(u)->second;
        dfs.push_back(v);
      }
    }
  }
}

Weight Tree::contract(const Graph &G) {
  Weight w = 0;
  stack<Vertex> next;
  next.push(root);
  while (!next.empty()) {
    Vertex current = next.top();
    next.pop();
    if (G.adjList[current].size() == 0) {
      assert(tree[current].children.size() == 1);
      Vertex child = *tree[current].children.begin();

      next.push(child);
      tree[child].weight += tree[current].weight;
      tree[child].parent = tree[current].parent;
      tree[tree[current].parent].children.insert(child);

      tree[tree[current].parent].remove(current);
      tree[current].parent = -2;
      tree[current].weight = 0;
      tree[current].children.clear();
    } else {
      w += tree[current].weight;
      for (set<Vertex>::iterator it = tree[current].children.begin();
           it != tree[current].children.end(); ++it) {
        next.push(*it);
      }
    }
  }

  return w;
}
