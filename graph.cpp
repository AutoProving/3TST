#include "graph.hpp"

using namespace std;

Weight MAX_WEIGHT =
    LONG_MAX; // Initialization of the global variable with maximum weight

Graph::Graph(istream &input) {
  std::string line;
  int numberEdges, numberTerminals, numberVertices;

  // Find graph section
  find_next(input, "SECTION Graph", line);

  // Get number of vertices
  getline(input, line);
  std::stringstream(line).ignore(256, ' ') >> (numberVertices);
  adjList.reserve(numberVertices);
  for (int i = 0; i < numberVertices; i++) {
    std::map<Vertex, Weight> tmpmap; // creates an empty map.
    adjList.push_back(tmpmap);
  }

  // Get number of edges
  getline(input, line);
  std::stringstream(line).ignore(256, ' ') >> (numberEdges);

  // Read edges
  for (int i = 0; i < (numberEdges); ++i) {
    Vertex v1, v2;
    Weight w;
    getline(input, line);
    std::stringstream(line).ignore(256, ' ') >> v1 >> v2 >> w;
    if (v1 != v2) {
      v1--;
      v2--;
      adjList[v1].insert(std::pair<Vertex, Weight>(v2, w));
      adjList[v2].insert(std::pair<Vertex, Weight>(v1, w));
    }
  }

  // Get terminals section
  find_next(input, "SECTION Terminals", line);

  // Get the number of terminals
  getline(input, line);
  std::stringstream(line).ignore(256, ' ') >> (numberTerminals);

  // initialize Terminals
  terminals.resize(numberTerminals, 0);
  // all entries in terminalsMap are initialized to -1
  terminalsMap.resize(numberVertices, -1);
  for (int i = 0; i < numberTerminals; ++i) {
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

void Graph::print() {
  cout << endl;
  cout << "printing Graph" << endl;
  cout << endl;
  cout << "Number of Vertices: " << adjList.size() << endl;
  cout << "Number of Terminals: " << terminals.size() << endl;
  cout << endl;
  cout << "Terminals: " << endl;
  // print Terminals
  for (unsigned int i = 0; i < terminals.size(); i++) {
    cout << terminals[i] + 1 << " "; // adds +1 back to the vertex number
  }
  cout << endl << endl;
  // print Edges
  for (unsigned int v1 = 0; v1 < adjList.size(); v1++) {
    for (std::map<Vertex, Weight>::iterator it = adjList[v1].begin();
         it != adjList[v1].end(); ++it) {
      // Obs: When printing, adds 1 back to the vertex number.
      cout << "(" << v1 + 1 << "," << (*it).first + 1 << "," << (*it).second
           << ")" << endl;
    }
  }
}

map<pair<Vertex, Vertex>, vector<Vertex>> Graph::contract() {
  map<pair<Vertex, Vertex>, vector<Vertex>> hash;
  // Contract vertex of degree 1
  for (unsigned int v = 0; v < adjList.size(); ++v) {
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
  for (unsigned int v = 0; v < adjList.size(); ++v) {
    if (terminalsMap[v] == -1 && adjList[v].size() == 2) {
      Vertex v1 = adjList[v].begin()->first;
      Weight w = adjList[v].begin()->second;
      Vertex v2 = adjList[v].rbegin()->first;
      w += adjList[v].rbegin()->second;

      adjList[v].clear();

      vector<Vertex> tmp1;
      vector<Vertex> tmp2;
      tmp1.push_back(v);
      Vertex old_v1 = v;
      Vertex old_v2 = v;
      adjList[v1].erase(old_v1);
      adjList[v2].erase(old_v2);
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

      while ((terminalsMap[v1] == -1 && adjList[v1].size() == 1) ||
             (terminalsMap[v2] == -1 && adjList[v2].size() == 1)) {
        while (terminalsMap[v1] == -1 && adjList[v1].size() == 1) {
          old_v1 = v1;
          w += adjList[v1].begin()->second;
          v1 = adjList[v1].begin()->first;
          adjList[old_v1].clear();
          adjList[v1].erase(old_v1);
          tmp1.push_back(old_v1);
          it = hash.find({old_v1, v1});
          if (it != hash.end()) {
            tmp1.insert(tmp1.end(), it->second.begin(), it->second.end());
            hash.erase(it);
            hash.erase(hash.find({v1, old_v1}));
          }
        }

        while (terminalsMap[v2] == -1 && adjList[v2].size() == 1) {
          old_v2 = v2;
          w += adjList[v2].begin()->second;
          v2 = adjList[v2].begin()->first;
          adjList[old_v2].clear();
          adjList[v2].erase(old_v2);
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
          if (itn->second > w) {
            adjList[v1].erase(itn);
            adjList[v2].erase(adjList[v2].find(v1));
            it = hash.find({v1, v2});
            if (it != hash.end()) {
              hash.erase(it);
              hash.erase(hash.find({v2, v1}));
            }
          }
        }
      }

      if (v1 != v2) {
        auto tmp = vector<Vertex>(tmp1.rbegin(), tmp1.rend());
        tmp.insert(tmp.end(), tmp2.begin(), tmp2.end());
        map<Vertex, Weight>::iterator itn = adjList[v1].find(v2);
        if (itn == adjList[v1].end()) {
          adjList[v1].insert({v2, w});
          adjList[v2].insert({v1, w});
          hash.insert({{v1, v2}, tmp});
          hash.insert({{v2, v1}, vector<Vertex>(tmp.rbegin(), tmp.rend())});
        } else {
          if (itn->second > w) {
            adjList[v1].erase(itn);
            adjList[v1].insert({v2, w});
            adjList[v1].insert({v2, w});
            hash.erase({v1, v2});
            hash.insert({{v1, v2}, tmp});
            adjList[v2].erase(v1);
            adjList[v2].insert({v1, w});
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
