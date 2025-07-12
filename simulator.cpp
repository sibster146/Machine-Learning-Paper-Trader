#include <fstream>
#include <iostream>
#include <string>
#include "orderbook.hpp"
#include "json.hpp"  // nlohmann/json
#include "simulator.hpp"
#include "model.hpp"
#include <fstream>
#include <sstream>
#include "position.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>

std::vector<long double> feature_engineering(std::pair
    <std::vector<std::pair<long double,long double>>,std::vector<std::pair<long double,long double>>>& asks_bids_pair)
{
    std::vector<long double> feature_vector;
    std::vector<std::pair<long double,long double>>& asks = asks_bids_pair.first;
    std::vector<std::pair<long double,long double>>& bids = asks_bids_pair.second;
    
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


Simulator::Simulator() = default;



std::string get_current_datetime() {
    auto now = std::chrono::system_clock::now();
    std::time_t t_now = std::chrono::system_clock::to_time_t(now);
    std::tm* local_tm = std::localtime(&t_now);

    std::ostringstream oss;
    oss << std::put_time(local_tm, "%Y_%m_%d_%H_%M_%S");
    return oss.str();
}

void Simulator::simulate_real_time(std::string product_id, std::string duration, LogisticRegressionModel model, double confidence_limit, int update_lag,long double buy_amount){
    // Spawn Python WebSocket writer
    std::string python_exec = "python3 websocket.py " + product_id + " " + duration + " &";
    std::system(python_exec.c_str());  // Start Python in background (&)

    // Open FIFO for reading (will block until writer opens it)
    std::ifstream pipe("/tmp/datapipeline");
    if (!pipe.is_open()) {
        std::cerr << "Failed to open named pipe." << std::endl;
        return;
    }

    // Open log file for completed trades
    std::string date = get_current_datetime();
    std::string filename = "simulations/" + date + product_id + ".csv";
    std::ofstream log_file(filename);
    log_file << "buy_sequence_number,sell_sequence_number,buy_amount,sell_amount,volume,profit\n";
    log_file.flush();

    OrderBook orderbook;

    std::string line;
    while (pipe.is_open() && std::getline(pipe, line)) {
        try {
            nlohmann::json line_json = nlohmann::json::parse(line);
            nlohmann::json str_sequence_number = line_json["sequence_num"];
            int sequence_number = static_cast<int>(str_sequence_number);

            while ((!positions.empty()) && (positions.front().get_sell_sequence_number() <= sequence_number)){
                Position& sell_position = positions.front();
                const long double sell_volume = sell_position.get_volume();
                const long double buy_amount = sell_position.get_buy_amount();
                const long double sell_amount = orderbook.get_sell_amount_by_sell_volume(sell_volume);
                sell_position.update_position(sell_amount);
                pnl += (sell_amount - buy_amount);
                positions.pop_front();
                std::cout << "PNL: " << pnl << std::endl;

                if (log_file.is_open()){
                    log_file << sell_position.print_to_log() +"\n";
                    log_file.flush();
                    // std::cout << log << std::endl;
                }
                else {
                    std::cerr << "Unable to open file\n";
                }
            }


            nlohmann::json updates = line_json["events"][0]["updates"];
            orderbook.process_updates(updates);
            std::pair<std::vector<std::pair<long double,long double>>, std::vector<std::pair<long double,long double>>> ask_bid_pair = orderbook.get_n_bids_asks(10); // HARDCODED PRICE LEVEL

            // INFERENCE HERE
            std::vector<long double> inference_vector = feature_engineering(ask_bid_pair);
            double probability = model.predict_probability(inference_vector);
            std::cout << probability << std::endl;

            if (probability >= confidence_limit){
                long double buy_volume = orderbook.get_buy_volume_by_buy_amount(buy_amount);
                Position buy_position{buy_amount, buy_volume, sequence_number, sequence_number + update_lag};
                positions.push_back(buy_position);
            }
            

        } catch (const std::exception& e) {
            std::cerr << "Parse error or missing key: " << e.what() << "\nLine: " << line << std::endl;
        }
    }

    log_file.close();
    pipe.close();
}
