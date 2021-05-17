#pragma once

#include <iostream>
#include <stack>

#include "graph.hpp"

using namespace std;

class treeNode {
public:
  Vertex parent;
  Weight weight;
  set<Vertex> children;
  treeNode() : parent{-2}, weight{0} {};
  int pruneLeaves(const vector<int> &terminalsMap);
  void remove(Vertex v);
};

class Tree {
public:
  Vertex root;
  vector<treeNode> tree;

  Tree(const Graph &G, istream &input);
  Tree(const Graph &G, Vertex root);

  void print(); // Recursively prints the tree
  void print(const map<pair<Vertex, Vertex>, vector<Vertex>>
                 &hash); // Recursively prints the tree
  void pruneRoot(const vector<int> &terminalsMap); // Prune the root if it's not
                                                   // a terminal node
  bool check(const Graph &G);
  Weight pruneLeaves(
      const vector<int>
          &terminalsMap); // Prune useless leaves and return the size of the tree
  int size();
};
