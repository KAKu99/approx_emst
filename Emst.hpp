#include <Eigen/Dense>
#include <iostream>
#include <list>
#include <stack>
#include <vector>
#include <map>
#include <set>

using namespace std;
using namespace Eigen;

#ifndef EMST_HPP
#define EMST_HPP
class Emst{
    public:
        Emst(MatrixXd inputData) : data(inputData) {}
        void ComputeMST(int k_init, int num_threads);
        void OutputCSV();
        void OutputLength();
    private:
        MatrixXd data;
        MatrixXd mstResults;
};


#endif // EMST_HPP


#ifndef GRAPH_HPP
#define GRAPH_HPP

// graph
class Graph {
public:
    int V; // Number of vertices
    list<pair<int, double>>* adj;
    set<pair<int, int>> edges; // Set to store edges and avoid duplicates

    Graph(int V) {
        this->V = V;
        adj = new list<pair<int, double>>[V];
    }

    // Add an edge to the graph, avoiding duplicates
    void addEdge(int v, int w, double weight) {
        // Check if the edge already exists
        if (edges.find({min(v, w), max(v, w)}) == edges.end()) {
            adj[v].push_back(make_pair(w, weight));
            adj[w].push_back(make_pair(v, weight)); // Since it's an undirected graph
            edges.insert({min(v, w), max(v, w)});   // Add the edge to the set
        }
    }

    void DFSUtil(int v, vector<bool>& visited) {
        stack<int> s;
        s.push(v);

        while (!s.empty()) {
            int curr = s.top();
            s.pop();

            if (curr < 0 || curr >= V) {
                cerr << "Error: Invalid vertex idx " << curr << endl;
                continue;
            }

            if (!visited[curr]) {
                visited[curr] = true;

                for (auto& neighbor : adj[curr]) {
                    int nextVertex = neighbor.first;
                    if (nextVertex < 0 || nextVertex >= V) {
                        cerr << "Error: Invalid nextVertex idx " << nextVertex << endl;
                        continue;
                    }
                    if (!visited[nextVertex]) {
                        s.push(nextVertex);
                    }
                }
            }
        }
    }

    // Function to check if the graph is connected
    bool isConnected() {
        vector<bool> visited(V, false);
            // エッジを持つ最初の頂点を探す
        int startVertex = -1;

        for (int i = 0; i < V; ++i) {
            if (adj == nullptr) {
                cerr << "Error: adj does not init." << endl;
                exit(EXIT_FAILURE);
            }
            if (!adj[i].empty()) {
                startVertex = i;
                break;
            }
        }

        if (startVertex == -1) {
            cerr << "Error: Any vertex doesn't have edges" << endl;
            return false;
        }
        // Find the first vertex with an edge
        // int startVertex = -1;
        for (int i = 0; i < V; ++i) {
            if (!adj[i].empty()) {
                startVertex = i;
                break;
            }
        }

        // If no edges, the graph is isolated
        if (startVertex == -1) {
            return false;
        }
        cout << "DFS" << endl;
        // Perform DFS from the first vertex
        DFSUtil(startVertex, visited);
        cout << "DFS end" << endl;
        // Check if all vertices with edges were visited
        for (int i = 0; i < V; ++i) {
            if (!adj[i].empty() && !visited[i]) {
                return false;
            }
        }

        return true;
    }
};


#endif // GRAPH_HPP

