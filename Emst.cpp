#include <vector>
#include <fstream>
#include <iostream>
#include <chrono>
#include <utility>
#include "graphGenerator.hpp"
#include "kraskal.hpp"

using namespace std;

/**
* @brief Compute the Minimum Spanning Tree and store the results(mstResults)
*/

// Function to find leaf nodes in a forest (multiple MSTs)
vector<int> findLeafNodes(const MatrixXd& mst) {
    map<int, int> degree; // Map to store the degree of each node
    vector<pair<int,int>> edges;

    // Convert the MST to a list of edges
    for (int i = 0; i < mst.rows(); ++i) {
        edges.push_back({mst(i,0), mst(i,1)});        
    }

    // Calculate the degree of each node
    for (const auto& edge : edges) {
        int u = edge.first;
        int v = edge.second;

        // Increment the degree for both nodes in the edge
        degree[u]++;
        degree[v]++;
    }

    // Find the nodes with degree 1 (leaf nodes)
    vector<int> leafNodes;
    for (const auto& entry : degree) {
        if (entry.second == 1) {
            leafNodes.push_back(entry.first);
        }
    }

    return leafNodes;
}


void Emst::ComputeMST(int k_init, int num_threads) {
    cout << "data size: " << data.rows() << " x " << data.cols() << endl;
    cout << "Start computing EMST..." << endl;
    auto start = chrono::high_resolution_clock::now();
    // Generate the graph from the data
    Graph graph_of_data(data.rows());
    // gen_graph(data, graph_of_data);
    vector<float> flat_data = flatten_data(data);

    auto start_hnsw = chrono::high_resolution_clock::now();
    HNSWIndex hnsw_index(data.cols(),32,flat_data);
    auto end_hnsw = chrono::high_resolution_clock::now();
    double elapsed_hnsw = chrono::duration_cast<chrono::microseconds>(end_hnsw - start_hnsw).count();
    double elapsed_hnsw_sec = elapsed_hnsw / 1000000.0;
    cout << "HNSW Index Elapsed time: " << elapsed_hnsw_sec << " sec" << endl;
    int k = k_init;
    faiss_NNDescent(data, k, 15, &graph_of_data);
    
    bool subgraph_exist = true;// dev temporary
    int hnsw_k = k;
    int loopcount = 0;
    while(subgraph_exist){
        loopcount++;
        // update the k <- 2 * k
        hnsw_k = 2 * hnsw_k;
        hnsw_index.setEfSearch(hnsw_k);// 2 times of previous hnsw_k

        cout << "Update hnsw k to " << hnsw_k << endl;
        // build the temporary MST by using the kratkal algorithm
        cout << "Find temporary MST..." << endl;
        cout << "edge size: " << graph_of_data.edges.size() << endl;
        auto start_kraskal = chrono::high_resolution_clock::now();
        MatrixXd tmp_mst = kraskal(&graph_of_data);
        auto end_kraskal = chrono::high_resolution_clock::now();
        double elapsed_kraskal = chrono::duration_cast<chrono::microseconds>(end_kraskal - start_kraskal).count();
        double elapsed_kraskal_sec = elapsed_kraskal / 1000000.0;
        cout << "Kraskal Elapsed time: " << elapsed_kraskal_sec << " sec" << endl;

        // find the 1 degree node from tmp_mst(Candidate node for the new edge)
        cout << "Search degree_1_nodes..." << endl;
        auto start_findLeafNodes = chrono::high_resolution_clock::now();
        vector<int> degree_1_nodes;
        degree_1_nodes = findLeafNodes(tmp_mst);
        auto end_findLeafNodes = chrono::high_resolution_clock::now();
        double elapsed_findLeafNodes = chrono::duration_cast<chrono::microseconds>(end_findLeafNodes - start_findLeafNodes).count();
        double elapsed_findLeafNodes_sec = elapsed_findLeafNodes / 1000000.0;
        cout << "Find degree_1_nodes Elapsed time: " << elapsed_findLeafNodes_sec << " sec" << endl;

        // find the additional edge by using the AkNNS on HNSW, and merge the edge to the graph
        cout << "Find additional edges on HNSW..." << endl;
        vector<pair<int,int>> additional_edges;
        additional_edges = hnsw_index.search_and_merge(data,degree_1_nodes, hnsw_k, &graph_of_data);

        // Check whether the mstResults contains any subgraph
        cout << "Check whether the graph contains any subgraph..." << endl;
        subgraph_exist = !graph_of_data.isConnected();
        cout << "subgraph_exist: " << subgraph_exist << endl;
        // subgraph_exist = true;// dev temporary
        if(subgraph_exist){
            cout << "loopcount: " << loopcount << endl;
            cout << "Subgraph exist. Continue to loop..." << endl;
        }else{
            cout << "loopcount: " << loopcount << endl;
            cout << "Subgraph does not exist. Finish to loop..." << endl;
            
        }
    }
    // Compute the Minimum Spanning Tree
    auto start_kraskal = chrono::high_resolution_clock::now();
    mstResults = kraskal(&graph_of_data);
    auto end_kraskal = chrono::high_resolution_clock::now();
    double elapsed_kraskal = chrono::duration_cast<chrono::microseconds>(end_kraskal - start_kraskal).count();
    double elapsed_kraskal_sec = elapsed_kraskal / 1000000.0;
    cout << "Kraskal Elapsed time: " << elapsed_kraskal_sec << " sec" << endl;
    


    auto end = chrono::high_resolution_clock::now();
    double elapsed = chrono::duration_cast<chrono::microseconds>(end - start).count();
    double elapsed_sec = elapsed / 1000000.0;
    // end sign
    cout << "End computing EMST." << endl;
    cout << "Elapsed time: " << elapsed_sec << " sec" << endl;
    
    
    if(bool save_graph = true){
        string file_path = "graph.csv";
        ofstream outputfile(file_path);

        if (!outputfile.is_open()) {
            cerr << "Failed to open graph file for writing." << endl;
        }
        for (int i = 0; i < data.rows(); i++) {
            for (auto x : graph_of_data.adj[i]) {
                outputfile << i << "," << x.first << "," << x.second << endl;
            }
        }
        outputfile.close();
        cout << "Graph data written to " << file_path << endl;
    }

}

// Output the EMST length
void Emst::OutputLength() {
    double total = 0;
    for (int i = 0; i < mstResults.rows(); i++) {
        VectorXd a = data.row(int(mstResults(i,0)));
        VectorXd b = data.row(int(mstResults(i,1)));
        double weight = (a - b).norm();
        mstResults(i, 2) = weight;
        total = total + weight;
    }
    cout << "Total of edge weights: " << total << endl;
    cout << "Number of edges: " << mstResults.rows() << endl;
}


// Output the MST results to a CSV file
void Emst::OutputCSV() {
    string file_path = "mst.csv";
    ofstream outputfile(file_path);

    if (!outputfile.is_open()) {
        cerr << "Failed to open graph file for writing." << endl;
    }
    for (int i = 0; i < mstResults.rows(); i++) {
        outputfile << mstResults(i,0) << "," << mstResults(i,1) << "," << mstResults(i,2) << endl;
    }
    outputfile.close();
    cout << "mst data written to " << file_path << endl;
}
