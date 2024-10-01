#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

// Read fvecs file
vector<vector<float>> read_fvecs(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + filename);
    }

    vector<vector<float>> data;
    
    while (file) {
        int dim;  
        file.read(reinterpret_cast<char*>(&dim), sizeof(int));
        if (!file) break;
        vector<float> vec(dim);
        file.read(reinterpret_cast<char*>(vec.data()), dim * sizeof(float));

        if (!file) break;
        data.push_back(move(vec));
    }

    file.close();

    return data;
}

#include <Eigen/Dense>

using namespace Eigen;

const int IMAGE_SIZE = 28 * 28; // MNIST

// mnist loader
MatrixXd read_mnist_images(const string& filename, int num_images) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + filename);
    }

    file.seekg(16);

    MatrixXd images(num_images, IMAGE_SIZE);

    for (int i = 0; i < num_images; ++i) {
        vector<unsigned char> temp(IMAGE_SIZE);
        file.read(reinterpret_cast<char*>(temp.data()), IMAGE_SIZE);

        for (int j = 0; j < IMAGE_SIZE; ++j) {
            images(i, j) = static_cast<double>(temp[j]);
        }
    }

    return images;
}

// fasttext loader (Only for vectors)
vector<vector<float>> read_fasttext(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + filename);
    }

    vector<vector<float>> vectors;
    string line;
    int wordCount, dim;

    file >> wordCount >> dim;
    getline(file, line);

    while (getline(file, line)) {
        istringstream iss(line);
        string word;
        iss >> word;
        vector<float> vec(dim);
        for (int i = 0; i < dim; ++i) {
            iss >> vec[i];
        }

        vectors.push_back(move(vec));
    }

    file.close();
    return vectors;
}

// glove loader (Only for vectors)
vector<vector<float>> read_glove(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open file: " + filename);
    }

    vector<vector<float>> vectors;
    string line;
    int wordCount, dim;
    dim = 200;  // GloVe 200d

    while (getline(file, line)) {
        istringstream iss(line);
        string word;
        iss >> word;

        vector<float> vec(dim);
        for (int i = 0; i < dim; ++i) {
            iss >> vec[i];
        }

        vectors.push_back(move(vec));
    }

    file.close();
    return vectors;
}

