#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>

std::vector<std::vector<float>> get_top_n_percent(std::vector<std::vector<float>>& data, float percentage, unsigned int seed = 2024) {
    if (data.empty()) {
        std::cerr << "data is empty" << std::endl;
        return {};
    }

    std::mt19937 gen(seed); 

    std::shuffle(data.begin(), data.end(), gen);

    // Pick top N% of data
    size_t num_samples = static_cast<size_t>(data.size() * percentage / 100.0);
    if (num_samples > data.size()) {
        num_samples = data.size();
    }

    return std::vector<std::vector<float>>(data.begin(), data.begin() + num_samples);
}

