## Overview

This is Approximation algorithm for the EMST(Euclidean Minimum Spanning Tree) problem. It provides features such as:

- **For High-dimention and Million Scale Datasets**: This algorithm is designed for high-dimention. We conduct experiments on high-dimention and million scale real-world datasets.
- **High-Quality Result**: It achieves a high-precision approximation of the EMST length and produces a structure similar to the exact result.
- **Parallelizable**: The part of the algorithm that accounts for the majority of the computational workload is implemented by FAISS. This portion is parallelizable.
## Prerequisites (Required Libraries)

List the libraries or software that need to be installed for this project to run.

- **faiss-cpu**: version 1.8.0
- **Eigen**: version 3.4.0
  
## Installation

Follow these steps to install and run the project locally.

1. Clone the repository to your local machine:
    ```bash
    git clone https://github.com/KAKu99/approx_emst_2024.git
    ```

2. Navigate into the project directory:
    ```bash
    cd approx_emst_2024
    ```
3. Build
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
## Usage
The data folder structure. Each dataset must be prepared beforehand.
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
This is example command. You can change settings in 'exp.sh'.
```bash
# Example command to run the project
bash exp.sh your/data/folder/path
```


