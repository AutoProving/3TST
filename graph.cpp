#include "graph.h"

using namespace std;

Weight MAX_WEIGHT =
    INT_MAX; // Initialization of the global variable with maximum weight

Graph::Graph(istream &input) {
  std::string line;

  // Find graph section
  find_next(input, "SECTION Graph", line);

  // Get number of vertices
  getline(input, line);
  std::stringstream(line).ignore(256, ' ') >> (this->numberVertices);
  adjList.reserve(this->numberVertices);
  for (int i = 0; i < this->numberVertices; i++) {
    std::map<Vertex, Weight> tmpmap; // creates an empty map.
    adjList.push_back(tmpmap);
  }

  // Get number of edges
  getline(input, line);
  std::stringstream(line).ignore(256, ' ') >> (this->numberEdges);

  // Read edges
  for (int i = 0; i < (this->numberEdges); ++i) {
    Vertex v1, v2;
    Weight w;
    getline(input, line);
    std::stringstream(line).ignore(256, ' ') >> v1 >> v2 >> w;
    v1--;
    v2--;
    adjList[v1].insert(std::pair<Vertex, Weight>(v2, w));
    adjList[v2].insert(std::pair<Vertex, Weight>(v1, w));
  }

  // Get terminals section
  find_next(input, "SECTION Terminals", line);

  // Get the number of terminals
  getline(input, line);
  std::stringstream(line).ignore(256, ' ') >> (this->numberTerminals);

  // initialize Terminals
  terminals.resize(this->numberTerminals, 0);
  // all entries in terminalsMap are initialized to -1
  terminalsMap.resize(this->numberVertices, -1);
  for (int i = 0; i < this->numberTerminals; ++i) {
    getline(input, line);
    std::stringstream(line).ignore(256, ' ') >> terminals[i];
    terminals[i]--; // subtracts 1 because vertices are numbered from 0
    terminalsMap[terminals[i]] = i;
  }
}

bool Graph::find_next(std::istream &input, const std::string &pat,
                      std::string &line) {
  while (getline(input, line)) {
    if (line == pat) {
      return true;
    }
  }
  return false;
}

void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              Vertex source) {
  min_distance[source] = 0;
  set<pair<Weight, Vertex>> active_vertices;
  active_vertices.insert({0, source});

  while (!active_vertices.empty()) {
    Vertex where = active_vertices.begin()->second;
    active_vertices.erase(active_vertices.begin());
    for (std::map<Vertex, Weight>::const_iterator it = adjList[where].begin();
         it != adjList[where].end(); ++it)
      if (min_distance[it->first] > min_distance[where] + it->second) {
        active_vertices.erase({min_distance[it->first], it->first});
        min_distance[it->first] = min_distance[where] + it->second;
        origin[it->first] = where;
        active_vertices.insert({min_distance[it->first], it->first});
      }
  }
}

void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              set<pair<Weight, Vertex>> &active_vertices) {

  while (!active_vertices.empty()) {
    Vertex where = active_vertices.begin()->second;
    active_vertices.erase(active_vertices.begin());
    for (std::map<Vertex, Weight>::const_iterator it = adjList[where].begin();
         it != adjList[where].end(); ++it)
      if (min_distance[it->first] > min_distance[where] + it->second) {
        active_vertices.erase({min_distance[it->first], it->first});
        min_distance[it->first] = min_distance[where] + it->second;
        origin[it->first] = where;
        active_vertices.insert({min_distance[it->first], it->first});
      }
  }
}

Vertex dijkstra(const vector<map<Vertex, Weight>> &adjList,
                vector<Weight> &min_distance, vector<Vertex> &origin,
                Vertex source, const vector<int> &terminalsMap, int T) {
  min_distance[source] = 0;
  set<pair<Weight, Vertex>> active_vertices;
  active_vertices.insert({0, source});

  while (!active_vertices.empty()) {
    Vertex where = active_vertices.begin()->second;
    if (terminalsMap[where] != -1)
      --T;
    if (T == 0)
      return where;
    active_vertices.erase(active_vertices.begin());
    for (std::map<Vertex, Weight>::const_iterator it = adjList[where].begin();
         it != adjList[where].end(); ++it)
      if (min_distance[it->first] > min_distance[where] + it->second) {
        active_vertices.erase({min_distance[it->first], it->first});
        min_distance[it->first] = min_distance[where] + it->second;
        origin[it->first] = where;
        active_vertices.insert({min_distance[it->first], it->first});
      }
  }
  return -1;
}

Vertex dijkstra(const vector<map<Vertex, Weight>> &adjList,
                vector<Weight> &min_distance, vector<Vertex> &origin,
                vector<bool> &inTree,
                set<pair<Weight, Vertex>> &active_vertices) {

  while (!active_vertices.empty()) {
    Vertex where = active_vertices.begin()->second;
    if (inTree[where])
      return where;
    active_vertices.erase(active_vertices.begin());
    for (std::map<Vertex, Weight>::const_iterator it = adjList[where].begin();
         it != adjList[where].end(); ++it)
      if (min_distance[it->first] > min_distance[where] + it->second) {
        active_vertices.erase({min_distance[it->first], it->first});
        min_distance[it->first] = min_distance[where] + it->second;
        origin[it->first] = where;
        active_vertices.insert({min_distance[it->first], it->first});
      }
  }
  return -1;
}

void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              Vertex source, Vertex target) {
  min_distance[source] = 0;
  set<pair<Weight, Vertex>> active_vertices;
  active_vertices.insert({0, source});

  while (!active_vertices.empty()) {
    Vertex where = active_vertices.begin()->second;
    if (where == target)
      return;
    active_vertices.erase(active_vertices.begin());
    for (std::map<Vertex, Weight>::const_iterator it = adjList[where].begin();
         it != adjList[where].end(); ++it)
      if (min_distance[it->first] > min_distance[where] + it->second) {
        active_vertices.erase({min_distance[it->first], it->first});
        min_distance[it->first] = min_distance[where] + it->second;
        origin[it->first] = where;
        active_vertices.insert({min_distance[it->first], it->first});
      }
  }
}

void dijkstra(const vector<map<Vertex, Weight>> &adjList,
              vector<Weight> &min_distance, vector<Vertex> &origin,
              Vertex source, Vertex v1, Vertex v2) {
  min_distance[source] = 0;
  set<pair<Weight, Vertex>> active_vertices;
  active_vertices.insert({0, source});

  while (!active_vertices.empty()) {
    Vertex where = active_vertices.begin()->second;
    if (where == v1 || where == v2)
      return;
    active_vertices.erase(active_vertices.begin());
    for (std::map<Vertex, Weight>::const_iterator it = adjList[where].begin();
         it != adjList[where].end(); ++it)
      if (min_distance[it->first] > min_distance[where] + it->second) {
        active_vertices.erase({min_distance[it->first], it->first});
        min_distance[it->first] = min_distance[where] + it->second;
        origin[it->first] = where;
        active_vertices.insert({min_distance[it->first], it->first});
      }
  }
}

void Graph::print() {
  cout << endl;
  cout << "printing Graph" << endl;
  cout << endl;
  cout << "Number of Vertices: " << this->numberVertices << endl;
  cout << "Number of Edges: " << this->numberEdges << endl;
  cout << "Number of Terminals: " << this->numberTerminals << endl;
  cout << endl;
  cout << "Terminals: " << endl;
  // print Terminals
  for (int i = 0; i < numberTerminals; i++) {
    cout << this->terminals[i] + 1 << " "; // adds +1 back to the vertex number
  }
  cout << endl << endl;
  // print Edges
  for (Vertex v1 = 0; v1 < this->numberVertices; v1++) {
    for (std::map<Vertex, Weight>::iterator it = adjList[v1].begin();
         it != adjList[v1].end(); ++it) {
      // Obs: When printing, adds 1 back to the vertex number.
      cout << "(" << v1 + 1 << "," << (*it).first + 1 << "," << (*it).second
           << ")" << endl;
    }
  }
}

void check(const Graph &G) {
  for (Vertex v = 0; v < G.numberVertices; ++v) {
    for (auto it = G.adjList[v].begin(); it != G.adjList[v].end(); ++it) {
      auto tmp = G.adjList[it->first].find(v);
      if (tmp == G.adjList[it->first].end())
        cerr << v << " " << it->first << endl;
      assert(tmp != G.adjList[it->first].end());
      assert(tmp->second == it->second);
    }
  }
  vector<Weight> min_distance(G.adjList.size(), MAX_WEIGHT);
  vector<Vertex> origin(G.adjList.size(), -1);
  dijkstra(G.adjList, min_distance, origin, G.terminals.front());
  for (Vertex v = 0; v < G.numberVertices; ++v) {
    assert(min_distance[v] != MAX_WEIGHT || origin[v] == -1);
    assert(min_distance[v] == MAX_WEIGHT || origin[v] != -1 ||
           v == G.terminals.front());
    assert(min_distance[v] != MAX_WEIGHT || G.adjList[v].size() == 0);
    assert(min_distance[v] != MAX_WEIGHT || G.terminalsMap[v] == -1);
  }
}

map<pair<Vertex, Vertex>, vector<Vertex>> Graph::contract() {
  map<pair<Vertex, Vertex>, vector<Vertex>> hash;
  // Contract vertex of degree 1
  for (Vertex v = 0; v < numberVertices; ++v) {
    if (terminalsMap[v] == -1 && adjList[v].size() == 1) {
      Vertex current = v;
      while (terminalsMap[current] == -1 && adjList[current].size() == 1) {
        Vertex next = adjList[current].begin()->first;
        adjList[next].erase(current);
        adjList[current].clear();
        current = next;
      }
    }
  }

  // Contract vertex of degree 2
  for (Vertex v = 0; v < numberVertices; ++v) {
    if (terminalsMap[v] == -1 && adjList[v].size() == 2) {
      Vertex v1 = adjList[v].begin()->first;
      Weight w1 = adjList[v].begin()->second;
      Vertex v2 = adjList[v].rbegin()->first;
      Weight w2 = adjList[v].rbegin()->second;

      adjList[v].clear();

      vector<Vertex> tmp1;
      vector<Vertex> tmp2;
      tmp1.push_back(v);
      Vertex old_v1 = v;
      Vertex old_v2 = v;
      auto it = hash.find({old_v1, v1});
      if (it != hash.end()) {
        tmp1.insert(tmp1.end(), it->second.begin(), it->second.end());
        hash.erase(it);
        hash.erase(hash.find({v1, old_v1}));
      }
      it = hash.find({old_v2, v2});
      if (it != hash.end()) {
        tmp2.insert(tmp2.end(), it->second.begin(), it->second.end());
        hash.erase(it);
        hash.erase(hash.find({v2, old_v2}));
      }

      while ((terminalsMap[v1] == -1 && adjList[v1].size() == 2) ||
             (terminalsMap[v2] == -1 && adjList[v2].size() == 2)) {
        while (terminalsMap[v1] == -1 && adjList[v1].size() == 2) {
          if (adjList[v1].begin()->first == old_v1) {
            old_v1 = v1;
            w1 += adjList[v1].rbegin()->second;
            v1 = adjList[v1].rbegin()->first;
          } else {
            old_v1 = v1;
            w1 += adjList[v1].begin()->second;
            v1 = adjList[v1].begin()->first;
          }
          adjList[old_v1].clear();
          tmp1.push_back(old_v1);
          it = hash.find({old_v1, v1});
          if (it != hash.end()) {
            tmp1.insert(tmp1.end(), it->second.begin(), it->second.end());
            hash.erase(it);
            hash.erase(hash.find({v1, old_v1}));
          }
        }

        while (terminalsMap[v2] == -1 && adjList[v2].size() == 2) {
          if (adjList[v2].begin()->first == old_v2) {
            old_v2 = v2;
            w2 += adjList[v2].rbegin()->second;
            v2 = adjList[v2].rbegin()->first;
          } else {
            old_v2 = v2;
            w2 += adjList[v2].begin()->second;
            v2 = adjList[v2].begin()->first;
          }
          adjList[old_v2].clear();
          tmp2.push_back(old_v2);
          it = hash.find({old_v2, v2});
          if (it != hash.end()) {
            tmp2.insert(tmp2.end(), it->second.begin(), it->second.end());
            hash.erase(it);
            hash.erase(hash.find({v2, old_v2}));
          }
        }

        map<Vertex, Weight>::iterator itn = adjList[v1].find(v2);
        if (itn != adjList[v1].end() && v1 != v2) {
          if (itn->second > w1 + w2) {
            adjList[v1].erase(itn);
            auto it = adjList[v2].find(v1);
            if (it != adjList[v2].end())
              adjList[v2].erase(it);
          }
        }
      }
      adjList[v1].erase(old_v1);
      adjList[v2].erase(old_v2);

      if (v1 != v2) {
        auto tmp = vector<Vertex>(tmp1.rbegin(), tmp1.rend());
        tmp.insert(tmp.end(), tmp2.begin(), tmp2.end());
        map<Vertex, Weight>::iterator itn = adjList[v1].find(v2);
        if (itn == adjList[v1].end()) {
          adjList[v1].insert({v2, w1 + w2});
          adjList[v2].insert({v1, w1 + w2});
          hash.insert({{v1, v2}, tmp});
          hash.insert({{v2, v1}, vector<Vertex>(tmp.rbegin(), tmp.rend())});
        } else {
          if (itn->second > w1 + w2) {
            adjList[v1].erase(itn);
            adjList[v1].insert({v2, w1 + w2});
            hash.erase({v1, v2});
            hash.insert({{v1, v2}, tmp});
            adjList[v2].erase(v1);
            adjList[v2].insert({v1, w1 + w2});
            hash.erase({v2, v1});
            hash.insert({{v2, v1}, vector<Vertex>(tmp.rbegin(), tmp.rend())});
          }
        }
      } else {
          adjList[v1].erase(v2);
      }
    }
  }

  return hash;
}
