#include <cstddef>
#include <cstdint>
#include <map>
#include <unordered_map>

#include "tick.hpp"

// There is one book per intrument / venue

// Each book: price levels

// For each price level: 2 list of orders, orders by arrival time. One for each side: buy or sell

// Access: 
// - by price level
// - by order id (to cancel/modify an order)

// 1. Hash map from order id to Order node.

// 2. std::map to store price levels. One intrusive list per level.


struct Order{

    bool side;
    std::int64_t orderId;
    std::int64_t price; // in cents
    std::int64_t quantity; // remaining quantity


};


struct OrderNode{
    Order order;
    Order* prev{};
    Order* next{};

};

struct Level{
    std::int64_t price;

    OrderNode* order{};
};


template <std::size_t nlevels> //max number of price levels
class Book{

    private:

    // Map order Ids to Orders
    std::unordered_map<std::int64_t, OrderNode> _orders; // start with a unordered_map. Later switch to hashmap

    // Levels
    std::map<std::int64_t, Level> _levels;


    public:

    void on_tick(const Tick& tick){
        
    }
};



