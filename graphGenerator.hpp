#include <Eigen/Dense>
#include "Emst.hpp"

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <unordered_set>
#include <list>
#include <utility>
#include <faiss/IndexNNDescent.h>
#include <faiss/IndexFlat.h>
#include <faiss/IndexHNSW.h>
#include <omp.h>

#define THREADS 28

using namespace std;
using namespace Eigen;

float distance(const VectorXd& a, const VectorXd& b) {
    return (a - b).norm();
}


void faiss_NNDescent(const MatrixXd& data, int k, int max_iter, Graph* graph,int num_threads = 0) {
    bool debug = false;
    int n = data.rows();
    int dim = data.cols();
    // set number of threads (if not set by user then use maximum number of threads)
    if (num_threads == 0) num_threads = omp_get_max_threads();
    omp_set_num_threads(num_threads);
    
    // floatData is a vector of vectors
    vector<vector<float>> floatData(n, vector<float>(dim));
    vector<float> flatData(n * dim);
    // faiss::MyIndexNNDescentFlat index(dim, k, faiss::METRIC_L2);
    faiss::IndexNNDescentFlat index(dim, k, faiss::METRIC_L2);
    
    { // init faiss's NNDescent
        /* Paper Memo
            default K = 20 (NNDescent)
         */
        float rho = 1; // trade-off between speed and accuracy(paper said that rho = 0.5 is low-accuracy(high-speed) setting, rho = 1 is high-accuracy(low-speed) setting)
        index.nndescent.S = int(rho * k); // smapling size; in the paper, ρK, ρ (0,1]
        index.nndescent.R = int(rho * k); // smapling reverse link size; in the paper, ρK, ρ (0,1]
        // index.nndescent.R = 0;
        // index.nndescent.L = 2 * k; // pooling size on NN search(pooling L sample -> select S sample); default size is L = K + 50 (faiss impl), 
        index.nndescent.search_L = 15;
        index.nndescent.iter = max_iter;
        index.verbose = false;
    }


    { // MatrixXd to floatData
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < dim; j++) {
                floatData[i][j] = data(i, j);
            }
        }
    }

    { // Flatten floatData for adding to index
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < dim; ++j) {
                flatData[i * dim + j] = floatData[i][j];
            }
        }
        auto start_build = chrono::high_resolution_clock::now();
        index.add(n, flatData.data());// add data to index (build index)
        auto  end_build = chrono::high_resolution_clock::now();
        double elapsed_build = chrono::duration_cast<chrono::microseconds>(end_build - start_build).count();
        double elapsed_build_sec = elapsed_build / 1000000.0;
        cout << "KNNG Build Elapsed time: " << elapsed_build_sec << " sec" << endl;
    }
   
    
    vector<int> knng(k * n);
    knng = index.nndescent.final_graph;
    {// convert knn to graph
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < k; j++) {
                if (i != knng[i * k + j]) {
                    graph->addEdge(i, knng[i * k + j], distance(data.row(i), data.row(knng[i * k + j])));
                }
            }
        }
    }
}



// faiss HNSW
class HNSWIndex {
public:
    // constructor
    HNSWIndex(int d, int M, const vector<float>& data)
        : d(d), M(M), nb(data.size() / d) {
        // get mamimum number of threads
        int num_threads = omp_get_max_threads();
        omp_set_num_threads(num_threads);
        // create index
        index = new faiss::IndexHNSWFlat(d, M);
        // add data to index
        index->add(nb, data.data());
    }

    // destructor
    ~HNSWIndex() {
        delete index;
    }
    void setEfSearch(int ef_search) {
        index->hnsw.efSearch = ef_search;
    }

    // search k nearest neighbors. query is a index of data
    vector<pair<int, int>> search_and_merge(const MatrixXd& data, const vector<int>& query_idx, int k,Graph* graph) {
        int num_threads = THREADS; // number of threads
        omp_set_num_threads(num_threads);

        k = k + 1; // avoid itself
        int nq = query_idx.size();
        // print query_idx size
        cout << "query_idx size: " << nq << endl;
        vector<faiss::idx_t> I(k * nq);  // result index
        vector<float> D(k * nq);  // result distance

        // extract query data
        vector<float> query(nq * d);
        for (int i = 0; i < nq; i++) {
            // print query_idx[i]
            // cout << "query_idx[" << i << "]: " << query_idx[i] << endl;
            for (int j = 0; j < d; j++) {
                // cout << j << endl;
                query[i * d + j] = data(query_idx[i], j);
            }
        }
        // print query data set up
        cout << "query data complete" << endl;

        // search k-NN
        auto start_search = chrono::high_resolution_clock::now();
        index->search(nq, query.data(), k, D.data(), I.data());
        auto end_search = chrono::high_resolution_clock::now();
        double elapsed_search = chrono::duration_cast<chrono::microseconds>(end_search - start_search).count();
        double elapsed_search_sec = elapsed_search / 1000000.0;
        cout << "HNSW Search Elapsed time: " << elapsed_search_sec << " sec" << endl;
        // return the result (idx,idx, distance)
        vector<pair<int, int>> results;
        for (int i = 0; i < nq; i++) {
            for (int j = 1; j < k; j++) {// j = 1 means avoid itself
                graph->addEdge(query_idx[i], I[i * k + j], D[i * k + j]);
            }
        }

        return results;
    }

private:
    int d;  // dimension of data
    int M;  // size of the neighborhood
    int nb;  // number of data
    faiss::IndexHNSWFlat* index;  // index of HNSW 
};

// flatten the data from MatrixXd to vector<float>
vector<float> flatten_data(const MatrixXd& data) {
    vector<float> flat_data(data.size());
    for (int i = 0; i < data.rows(); i++) {
        for (int j = 0; j < data.cols(); j++) {
            flat_data[i * data.cols() + j] = data(i, j);
        }
    }
    return flat_data;
}