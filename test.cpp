#include <fstream>
#include <iostream>
#include <string>
#include "orderbook.hpp"
#include "json.hpp"  // nlohmann/json
#include "simulator.hpp"
#include "model.hpp"


std::vector<double> feature_engineering(std::pair
    <std::vector<std::pair<double,double>>,std::vector<std::pair<double,double>>>& asks_bids_pair, double buy_amount)
{
    std::vector<double> feature_vector;
    std::vector<std::pair<double,double>> asks = asks_bids_pair.first;
    std::vector<std::pair<double,double>> bids = asks_bids_pair.second;
    
    // Bid Ask Spread
    feature_vector.push_back(bids[0].first - asks[0].first);

    // Bid Volume Level 1, 2, 3
    for (int i = 0; i < 3; i++){
        feature_vector.push_back(bids[i].second);
    }
    
    // Ask Volume Level 1, 2, 3
    for (int i = 0; i < 3; i++){
        feature_vector.push_back(asks[i].second);
    }

    // Bid Price Level 1, 2, 3
    for (int i = 0; i < 3; i++){
        feature_vector.push_back(bids[i].first);
    }

    // Ask Price Level 1, 2, 3
    for (int i = 0; i < 3; i++){
        feature_vector.push_back(asks[i].first);
    }

    // Bid Volume Level Log 1, 2, 3, 4, 5
    for (int i = 0; i < 5; i++){
        feature_vector.push_back(std::log(bids[i].second));
    }

    // Ask Volume Level Log 1, 2, 3, 4, 5
    for (int i = 0; i < 5; i++){
        feature_vector.push_back(std::log(asks[i].second));
    }

    // Imbalance
    feature_vector.push_back((bids[0].second - asks[0].second)/(bids[0].second + asks[0].second));

    // VWAP Bid, Ask
    double bid_total_volume = 0;
    double ask_total_volume = 0;
    double bid_volume_weighted_sum = 0;
    double ask_volume_weighted_sum = 0;

    for (int i = 0; i < 10; i ++){
        bid_total_volume += bids[i].second;
        ask_total_volume += asks[i].second;
        bid_volume_weighted_sum += (bids[i].first * bids[i].second);
        ask_volume_weighted_sum += (asks[i].first * asks[i].second);
    }

    double bid_vwap = bid_volume_weighted_sum / bid_total_volume;
    double ask_vwap = ask_volume_weighted_sum / ask_total_volume;

    feature_vector.push_back(bid_vwap);
    feature_vector.push_back(ask_vwap);

    // VWAP Log Bid, Ask
    feature_vector.push_back(std::log(bid_vwap));
    feature_vector.push_back(std::log(ask_vwap));

    return feature_vector; 
}