#include <iostream>
#include <Eigen/Dense>
#include <cstdlib>
#include <fstream>
#include "dataloader.hpp"
#include "utils.hpp"
#include "Emst.hpp"

using namespace std;
using namespace Eigen;

int main(int argc, char* argv[]){
    const int required_args = 6;

    if (argc != required_args) {
        cout << "Usage: ./main <string data_folder_path> <int dataset_type; 0: MNIST, 1: SIFT, 2: fasttext, 3: glove> "
             << "<int percent_of_num_data; 0-100%> <int k; initial k> <int num_threads>" << endl;
        return 1;
    }

    string data_folder_path = argv[1];
    
    int dataset_type;
    istringstream(argv[2]) >> dataset_type;

    int percent_of_num_data;
    istringstream(argv[3]) >> percent_of_num_data;

    int k;
    istringstream(argv[4]) >> k;

    int num_threads;
    istringstream(argv[5]) >> num_threads;

    /* ############## dataset selector ###################### */
    /* I recommend to change the this part to your own enviroment */
    if (dataset_type < 0 || dataset_type > 3) {
        cout << "Error: dataset_type must be 0 (MNIST), 1 (SIFT), 2 (fasttext), or 3 (glove)." << endl;
        return 1;
    }
    /* ###################################################### */

    if (percent_of_num_data < 0 || percent_of_num_data > 100) {
        cout << "Error: percent_of_num_data must be between 0 and 100." << endl;
        return 1;
    }

    if (k <= 0) {
        cout << "Error: k must be a positive integer." << endl;
        return 1;
    }

    if (num_threads < 0) {
        cout << "Error: num_threads must be a positive integer." << endl;
        return 1;
    }

    cout << "Data folder path: " << data_folder_path << endl;
    cout << "Dataset type: " << dataset_type << endl;
    cout << "Percent of data: " << percent_of_num_data << "%" << endl;
    cout << "Initial k: " << k << endl;
    cout << "Number of threads: " << num_threads << endl;
    // initialize variables
    bool debug = false;
    MatrixXd data;
    MatrixXd tmp_data;
    string filename;
    vector<vector<float>> data_vec;


    /* ################################################ Data Loader ###################################################### 
    * 0: MNIST
    * 1: SIFT
    * 2: fasttext
    * 3: glove
    * ################################################################
    * #                                                              #
    * # I recommend to change the this part to your own data loader. #
    * #                                                              #
    * ################################################################
    */
    switch (dataset_type)
    {
    case 0:
        cout << "Dataset: MNIST" << endl;
        filename = data_folder_path + "/mnist/train-images-idx3-ubyte";  // MNIST file path
        tmp_data = read_mnist_images(filename, 60000); // Load MNIST images
        // convert to data_vec
        data_vec = vector<vector<float>>(tmp_data.rows(), vector<float>(tmp_data.cols()));
        for (int i = 0; i < tmp_data.rows(); i++) {
            for (int j = 0; j < tmp_data.cols(); j++) {
                data_vec[i][j] = tmp_data(i, j);
            }
        }

        // print first data and dataset name

        cout << "First data: " << endl;
        for (int i = 0; i < data_vec[0].size(); i++) {
            cout << data_vec[0][i] << " ";
        }
        break;
    case 1:
        cout << "Dataset: SIFT" << endl;
        filename = data_folder_path + "/sift/sift_base.fvecs";  // sift file path
        data_vec = read_fvecs(filename); // Load SIFT features
        // print first data and dataset name
        
        cout << "First data: " << endl;
        for (int i = 0; i < data_vec[0].size(); i++) {
            cout << data_vec[0][i] << " ";
        }
        break;
    case 2:
        cout << "Dataset: fasttext" << endl;
        filename = data_folder_path + "/fasttext/wiki-news-300d-1M.vec";  // fasttext file path
        data_vec = read_fasttext(filename); // Load fasttext features
        // print first data and dataset name
        cout << "First data: " << endl;
        for (int i = 0; i < data_vec[0].size(); i++) {
            cout << data_vec[0][i] << " ";
        }
        break;
    case 3:
        cout << "Dataset: glove" << endl;
        
        filename = data_folder_path + "/glove/twitter/glove.twitter.27B.200d.txt";  // glove file path
        data_vec = read_glove(filename); // Load glove features
        // print first data and dataset name
        cout << "First data: " << endl;
        for (int i = 0; i < data_vec[0].size(); i++) {
            cout << data_vec[0][i] << " ";
        }
        break;
    default:
        break;
    }
    /* ################################################ Data Loader ###################################################### */
    



    {// extract top N% of data (randomly shuffled)
        vector<vector<float>> data_part;
        data_part = get_top_n_percent(data_vec, percent_of_num_data); // Get top N% of data
        
        // Convert data to Eigen::MatrixXd
        data = MatrixXd(data_part.size(), data_part[0].size());
        
        for (int i = 0; i < data.rows(); i++) {
            for (int j = 0; j < data.cols(); j++) {
                data(i, j) = data_part[i][j];
            }
        }

        if(debug){
            cout << "Data: " << endl;
            cout << data << endl;
        
        }
    }

    Emst bs(data);
    // Compute MST
    bs.ComputeMST(k, num_threads);

    // Output the total length of the MST
    bs.OutputLength();
    // Output CSV file with MST edges
    bs.OutputCSV();

    return 0;
}
