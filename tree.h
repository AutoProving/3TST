#pragma once

#include <iostream>
#include <memory>
#include <stack>

#include "graph.h"

using namespace std;

class treeNode {
public:
  Vertex v;
  Weight w; // weight of edge (v,Parent->v); // Obs trows an error if such an
            // edge does not exist in the graph
  treeNode *parent; // NULL if root
  map<Vertex, shared_ptr<treeNode>> children;
  explicit treeNode(Vertex v)
      : v{v}, w{MAX_WEIGHT}, parent{NULL} {}; // Build a root
  treeNode(Vertex v, Weight w, treeNode *parent)
      : v{v}, w{w}, parent{parent} {};
  shared_ptr<treeNode> addChild(Vertex v, Weight w);
  shared_ptr<treeNode> addChild(shared_ptr<treeNode> N);
  void print();
  void printSubTree();
  int pruneLeaves(const vector<int> &terminalsMap);
};

class Tree {
public:
  const Graph &G; // Pointer to a graph
  const vector<int> &terminalsMap;
  const vector<Vertex> &terminals;
  shared_ptr<treeNode> root; // the root of the tree
  Tree(const Graph &G, Vertex v)
      : G{G}, terminalsMap{G.terminalsMap}, terminals{G.terminals},
        root{new treeNode(v)} {};
  Tree(const Graph &G, istream &input);
  explicit Tree(const Graph &G)
      : G{G}, terminalsMap{G.terminalsMap}, terminals{G.terminals} {};
  Tree(const Graph &G, treeNode &N)
      : G{G}, terminalsMap{G.terminalsMap}, terminals{G.terminals},
        root{new treeNode(N)} {};
  Tree(const Graph &G, Vertex v, const vector<int> &terminalsMap,
       const vector<Vertex> &terminals)
      : G{G}, terminalsMap{terminalsMap}, terminals{terminals},
        root{new treeNode(v)} {};
  Tree(const Graph &G, const vector<int> &terminalsMap,
       const vector<Vertex> &terminals)
      : G{G}, terminalsMap{terminalsMap}, terminals{terminals} {};
  Tree(const Graph &G, treeNode &N, const vector<int> &terminalsMap,
       const vector<Vertex> &terminals)
      : G{G}, terminalsMap{terminalsMap}, terminals{terminals},
        root{new treeNode(N)} {};
  Tree &operator=(const Tree &T) {
    root = T.root;
    return *this;
  };
  void print(); // Recursively prints the tree
  void print(const map<pair<Vertex, Vertex>, vector<Vertex>>
                 &hash); // Recursively prints the tree
  void pruneRoot();      // prune the root if it's not a terminal node
  Weight pruneLeaves();  // prune useless leaves en return the size of the tree
  int size();
};
