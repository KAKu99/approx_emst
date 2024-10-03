# Minimum required CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(MyProject VERSION 1.0)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add optimization and OpenMP flag
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fopenmp")

# Include directories (if needed)
# Add /usr/local/include to search path for headers
include_directories(/usr/local/include)

# Add the executable
add_executable(main.out
    main.cpp
    Emst.cpp
    graphGenerator.hpp
    kraskal.hpp
    dataloader.hpp
)

# Link libraries
target_link_libraries(main.out
    faiss
    lapack
    blas
)

# Set output directory
set_target_properties(main.out PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
