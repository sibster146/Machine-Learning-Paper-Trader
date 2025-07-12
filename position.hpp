#ifndef POSITION_H
#define POSITION_H

#include <string>
#include <ostream>
class Position{

    long double buy_amount;
    long double volume;
    long double sell_amount;
    long double profit;
    std::string order_id;
    int buy_sequence_number;
    int sell_sequence_number;

    public:
    Position(long double buy_amount, long double volume, int buy_sequence_number, int sell_sequence_number);
    const int get_sell_sequence_number();
    const long double get_volume() const;
    const long double get_buy_amount() const;
    const long double get_sell_amount() const;
    const std::string print_to_log() const;
    void update_position(long double sell_position);

};

#endif