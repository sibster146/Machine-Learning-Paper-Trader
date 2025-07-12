#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <vector>
#include "model.hpp"
#include <string>


LogisticRegressionModel::LogisticRegressionModel(std::string text_file){
    read_intercept_coefficient_file(text_file);
}


void LogisticRegressionModel::read_intercept_coefficient_file(std::string& text_file){
    std::ifstream infile(text_file);
    std::string line;

    if (!infile.is_open()) {
        std::cerr << "Failed to open file\n";
        return;
    }
    std::getline(infile, line);
    intercept = std::stod(line);
    while (std::getline(infile, line)) {
        coefficient_list.push_back(std::stod(line));
    }

    infile.close();
}

void LogisticRegressionModel::print_intercept_coefficient(){
    std::cout << "Intercept: " << intercept << std::endl;
    std::cout << "Coefficient: " << std::endl;
    for (auto& coef : coefficient_list){
        std::cout << coef << ", ";
    }
    std::cout << std::endl;
}

double LogisticRegressionModel::predict_probability(std::vector<long double>& inference_vector){
    double exp = 0;
    for(int i = 0; i < inference_vector.size(); i++){
        double coefficient = coefficient_list[i];
        double inference_feature = inference_vector[i];
        exp += (coefficient * inference_feature);
    }
    
    return 1.0 / (1.0 + std::exp(-(exp+intercept)));
}
