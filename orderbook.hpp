#ifndef ORDERBOOK_H
#define ORDERBOOK_H
#include <vector>
#include <utility>
#include "json.hpp"


class OrderBook {
    private:
    std::vector<std::pair<long double,long double>> asks;
    std::vector<std::pair<long double,long double>> bids;

    public:
    OrderBook();
    double get_mid_price();
    double get_buy_volume_by_buy_amount(long double buy_amount);
    double get_sell_amount_by_sell_volume(long double sell_volume);
    std::pair<std::vector<std::pair<long double,long double>>, std::vector<std::pair<long double,long double>>> get_n_bids_asks(int n);
    void process_updates(nlohmann::json updates);
    void process_level(nlohmann::json& update);
};

#endif