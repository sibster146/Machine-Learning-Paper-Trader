#include "position.hpp"

Position::Position(long double buy_amount, long double volume, int buy_sequence_number, int sell_sequence_number) :
    buy_amount(buy_amount), volume(volume), buy_sequence_number(buy_sequence_number), sell_sequence_number(sell_sequence_number) {}

const int Position::get_sell_sequence_number() {
    return sell_sequence_number;
}

const long double Position::get_volume() const {
    return volume;
}

const long double Position::get_buy_amount() const {
    return buy_amount;
}

const long double Position::get_sell_amount() const{
    return sell_amount;
}

const std::string Position::print_to_log() const {
    std::string res = (std::to_string(buy_sequence_number) + "," +
                        std::to_string(sell_sequence_number) + "," +
                        std::to_string(buy_amount) + "," +
                        std::to_string(sell_amount) + "," + 
                        std::to_string(volume) + "," +
                        std::to_string(profit)
                    );
    return res;
    }

void Position::update_position(long double sell_position){
    sell_amount = sell_position;
    profit = sell_amount - buy_amount;
}


// std::ostream& operator<<(std::ostream& os, const Position& position){
//     os << "Buy Amount: " << position.get_buy_amount() << ", Volume: " << position.get_volume() << std::endl;
//     return os;
// }



