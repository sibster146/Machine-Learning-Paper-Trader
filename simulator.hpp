#ifndef SIMULATOR_H
#define SIMULATOR_H
#include "orderbook.hpp"
#include "model.hpp"
#include <deque>
#include "position.hpp"



class Simulator{
    std::deque<Position> positions;
    long double pnl = 0;
    public:
    Simulator();
    void simulate_real_time(std::string product_id, std::string duration, LogisticRegressionModel model, double confidence_limit, int update_lag,long double buy_amount);
};

#endif