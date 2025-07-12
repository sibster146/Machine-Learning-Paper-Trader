#include "simulator.hpp"
#include "model.hpp"
#include <iostream>
int main(){
    std::cout << std::fixed << std::setprecision(8);  // globally apply 8 decimal digits
    Simulator simulator;
    double confidence_limit = .60;
    std::string model_file = "20_update_log_classifier.txt";
    LogisticRegressionModel model{model_file};
    int update_lag = 20;
    double buy_amount = 5.0;
    std::string product_id = "BTC-USD";
    std::string duration = "1800";
    // simulator.simulate_back_test("price_level_data/df2.csv");
    simulator.simulate_real_time(product_id, duration, model, confidence_limit, update_lag, buy_amount);
    std::cout << "simulation ended" << std::endl;

}