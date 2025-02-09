## Introduction
* This repository provides implementations of our algorithms for the EMST problem for high-dimensional large datasets.
* For details, please read our [paper]().
## Overview

This is an approximation algorithm for the Euclidean Minimum Spanning Tree (EMST) problem. It has the following features:

- **For high-dimensional large datasets**: This algorithm is specifically designed for high-dimensional data and has been tested on real-world datasets with millions of data points.
- **High-quality results**: It provides a high-recall approximation of the EMST length and generates a structure similar to the exact result.
- **Parallelizable**: The computational workload, which is the most intensive part of the algorithm, is implemented using FAISS and is parallelizable.

## Prerequisites (Required libraries)

The following libraries need to be installed for this project to run.

- **[faiss-cpu](https://github.com/facebookresearch/faiss)**: version 1.8.0
- **[Eigen](https://eigen.tuxfamily.org/index.php?title=Main_Page)**: version 3.4.0
  
## Installation

To install and run the project locally, please follow the following steps:

1. Clone the repository to your local machine:
    ```bash
    git clone https://github.com/KAKu99/approx_emst_2024.git
    ```

2. Navigate to the project directory:
    ```bash
    cd approx_emst_2024
    ```

3. Build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

## Usage

The data folder structure should be as follows. Each dataset must be prepared beforehand.

```bash
  ─── data/ # Directory for input datasets. Please input this path to exp.sh. 
   ├── fasttext/
   │ └── wiki-news-300d-1M.vec
   ├── glove/
   │ └── twitter/
   │  └── wiki-news-300d-1M.vec
   ├── mnist/
   │ └── train-images-idx3-ubyte
   └── sift/
     └── sift_base.fvecs
```

Here's an example command. You can adjust the settings in 'exp.sh'.

```bash
# Example command to run the project
bash exp.sh your/data/folder/path
```
## Citation
If you use our implementation, please cite the following paper.
```bash
@inproceedings{kido2025emst,
    title={None},  
    author={Kido, Keito and Amagata, Daichi and Hara, Takahiro},  
    booktitle={None},  
    pages={None},  
    year={None}  
}
```


## License
Copyright (c) 2024 Keito Kido  
This software is released under the [MIT license](https://github.com/KAKu99/approx_emst_2024/blob/master/LICENSE.txt).
