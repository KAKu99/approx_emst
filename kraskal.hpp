#include <iostream>
#include <Eigen/Dense>
#include <vector>
#include <algorithm>
#include <chrono>
#include <utility>
#include <list>
#include "Emst.hpp"

using namespace std;
using namespace Eigen;

// A structure to represent a weighted edge in graph

MatrixXd kraskal(Graph* graph){

    int V = graph->V;
    list<pair<int, double>>* adj = graph->adj;
    vector<pair<double, pair<int, int>>> edges;
    // Create a list of edges
    auto start = chrono::high_resolution_clock::now();
    for (int i = 0; i < V; i++) {
        for (auto x : adj[i]) {
            int v = x.first;
            double weight = x.second;
            edges.push_back({weight, {i, v}});
        }
    }
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
    cout << "Time to create edges: " << duration.count() << " microseconds" << endl;
    // Sort the edges by weight
    auto start_sort = chrono::high_resolution_clock::now();
    sort(edges.begin(), edges.end());
    auto end_sort = chrono::high_resolution_clock::now();
    auto duration_sort = chrono::duration_cast<chrono::microseconds>(end_sort - start_sort);
    cout << "Time to sort edges: " << duration_sort.count() << " microseconds" << endl;
    // Kruskal's algorithm
    vector<pair<int, int>> mst;
    double mst_weight = 0;
    vector<int> parent(V);
    vector<int> rank(V, 0); // Initialize rank array for Union-Find
    for (int i = 0; i < V; i++) {
        parent[i] = i;
    }

    // Find function with path compression
    function<int(int)> find = [&](int x) -> int {
        if (parent[x] != x) {
            parent[x] = find(parent[x]); // Path compression
        }
        return parent[x];
    };

    // Union function with union by rank
    auto union_sets = [&](int x, int y) {
        x = find(x);
        y = find(y);
        if (x != y) {
            if (rank[x] < rank[y]) {
                parent[x] = y;
            } else if (rank[x] > rank[y]) {
                parent[y] = x;
            } else {
                parent[y] = x;
                rank[x]++;
            }
        }
    };

    auto start_kraskal = chrono::high_resolution_clock::now();
    for (auto edge : edges) {
        double weight = edge.first;
        int u = edge.second.first;
        int v = edge.second.second;
        if (find(u) != find(v)) {
            mst.push_back({u, v});
            mst_weight += weight;
            union_sets(u, v);
        }
    }
    auto end_kraskal = chrono::high_resolution_clock::now();
    auto duration_kraskal = chrono::duration_cast<chrono::microseconds>(end_kraskal - start_kraskal);
    cout << "Time to run Kruskal's algorithm: " << duration_kraskal.count() << " microseconds" << endl;

    MatrixXd mstResults(mst.size(), 3);
    for (int i = 0; i < mst.size(); i++) {
        mstResults(i, 0) = mst[i].first;
        mstResults(i, 1) = mst[i].second;
        mstResults(i, 2) = 0;
    }
    return mstResults;
}
