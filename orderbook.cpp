#include <vector>
#include <utility>
#include "json.hpp"
#include "orderbook.hpp"
#include <iostream>

OrderBook::OrderBook() = default;

double OrderBook::get_mid_price(){
    long double best_ask = asks[0].first;
    long double best_bid = bids[0].first * -1;
    return (best_ask + best_bid) / 2;
}

double OrderBook::get_buy_volume_by_buy_amount(long double buy_amount){
    int i = 0;
    double long curr_buy_amount = 0;
    double long res_buy_volume = 0;

    while (i < asks.size() && curr_buy_amount < buy_amount){
        long double curr_price = asks[i].first;
        long double curr_volume = asks[i].second;
        long double curr_value = curr_price * curr_volume;
        if (curr_buy_amount + curr_value > buy_amount){
            long double needed_value = buy_amount - curr_buy_amount;
            res_buy_volume += (needed_value / curr_price);
            return res_buy_volume;
        }
        else {
            curr_buy_amount += curr_value;
            res_buy_volume += curr_volume;
            i+=1;
        }
    }

    return res_buy_volume;
}

double OrderBook::get_sell_amount_by_sell_volume(long double sell_volume){
    int i =0;
    long double res_sell_amount = 0;
    long double curr_sell_volume = 0;

    while (i < bids.size() && curr_sell_volume < sell_volume){
        long double curr_price = bids[i].first * -1;
        long double curr_volume = bids[i].second;
        long double curr_value = curr_price * curr_volume;

        if (curr_volume + curr_sell_volume > sell_volume){
            long double needed_volume = sell_volume - curr_sell_volume;
            res_sell_amount += (needed_volume * curr_price);
            return res_sell_amount;
        }
        else {
            curr_sell_volume += curr_volume;
            res_sell_amount += curr_value;
            i+=1;
        }

    }
    return res_sell_amount;
}

std::pair<std::vector<std::pair<long double,long double>>, std::vector<std::pair<long double,long double>>> OrderBook::get_n_bids_asks(int n){
    std::vector<std::pair<long double,long double>> n_asks(n);
    std::vector<std::pair<long double,long double>> n_bids(n);

    for (int i = 0; i < n; i++){
        n_asks[i] = asks[i];
        long double bid_price = bids[i].first * -1;
        long double bid_quantity = bids[i].second;
        n_bids[i] = std::make_pair(bid_price, bid_quantity);
    }

    return std::make_pair(n_asks,n_bids);

}

void OrderBook::process_updates(nlohmann::json updates){
    for (nlohmann::json& update : updates){
        process_level(update);
    }
}

int find_price_level_index(long double target_price, std::vector<std::pair<long double, long double>>& data){
    int l = 0;
    int r = data.size()-1;
    int m;
    while (l<=r){
        m = (l+r)/2;
        if (data[m].first == target_price){
            return m;
        }
        if (data[m].first > target_price){
            r = m-1;
        }
        else {
            l = m+1;
        }
    }
    return -1;
}

void OrderBook::process_level(nlohmann::json& update){
    std::string side = update["side"];
    std::string str_new_price = update["price_level"];
    std::string str_new_quantity = update["new_quantity"];
    long double new_price = std::stod(str_new_price);
    long double new_quantity = std::stod(str_new_quantity);
    if (side == "offer"){
        int price_level_index = find_price_level_index(new_price, asks);
        if (new_quantity > 0.0){
            if (price_level_index >= 0){
                asks[price_level_index] = std::make_pair(new_price, new_quantity);
            }
            else {
                auto it = std::lower_bound(
                    asks.begin(), 
                    asks.end(), 
                    new_price, 
                    [](const std::pair<long double, long double>& element, const long double value){
                        return element.first < value;
                    }
                );
                asks.insert(it, std::make_pair(new_price, new_quantity));
            }
        }
        else {
            if (price_level_index >= 0){
                asks.erase(asks.begin() + price_level_index);
            }
        }
    }
    else{
        new_price *=-1;
        int price_level_index = find_price_level_index(new_price, bids);
        if (new_quantity > 0.0){
            if (price_level_index >= 0){
                bids[price_level_index] = std::make_pair(new_price, new_quantity);
            }
            else {
                auto it = std::lower_bound(
                    bids.begin(), 
                    bids.end(), 
                    new_price, 
                    [](const std::pair<long double, long double>& element, const long double value){
                        return element.first < value;
                    }
                );
                bids.insert(it, std::make_pair(new_price, new_quantity));
            }
        }
        else {
            if (price_level_index >= 0){
                bids.erase(bids.begin() + price_level_index);
            }
        }
    }
}





