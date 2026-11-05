#include <cstddef>
#include <cstdint>
#include <map>
#include <unordered_map>

#include "tick.hpp"

// There is one book per intrument / venue / side

// Each book: price levels

// For each price level: 1 list of orders, sorted by arrival time. 

// Access: 
// - by price level
// - by order id (to cancel/modify an order)

// 1. Hash map from order id to Order node.

// 2. std::map to store price levels. One intrusive list per level.


// Intrusive list makes it easy to delete an order looked up by order Id
struct Order{

    bool side;
    std::int64_t orderId;
    std::int64_t price; // in cents
    std::int64_t quantity; // remaining quantity

    Order* prev{};
    Order* next{};

};

struct Level{

    std::int64_t order_count;
    Order* tail{};// This is the last order in the queue. New orders are inserted here.
};


// There is one such book per instrument / venue / side
template <std::size_t nlevels, bool buyside> 
class Book{

    private:

    // Map order Ids to Orders
    std::unordered_map<std::int64_t, OrderNode> _orders; // start with a unordered_map. Later switch to hashmap

    // Levels. Start with a map. TODO: move to a std::array<> indexed by price ticks and benchmark the difference.
    std::map<std::int64_t, Level> _levels;

    public:

    void on_tick(const Tick& tick){
        switch (tick.op)
        {
        case Operation::add:
            /* code */
            // Add new level if it did not exist
            Level& level = _levels[tick.price];

            if (tick.order_id > 0){[[likely]]
                Order& order = _orders[tick.order_id];//Create the new order in the hash map

                // Insert at the beginning of the price level
                if (level.order != nullptr)
                    level.order->prev = &order;

                level.order = &order;

            }


            break;
        case Operation::del:

            if (tick.order_id > 0){
                // find the order

                if (_orders.contains(tick.order_id)){

                    Order& order_to_delete = order[tick.order_id];

                    // Remove the order from the intrusive list in a level
                    if (order_to_delete.prev != nullptr)
                        order_to_delete.prev->next = order_to_delete.next;
                    else{
                        // It was the tail order: find its level
                        if (_levels.contains(tick.price)){
                            Level& level = _level[tick.price];
                            level.tail = order_to_delete.next;
                        }
                    }

                    if (order_to_delete.next != nullptr)
                        order_to_delete.next->prev = order_to_delete.prev;

                }
            }
            break;


        
        default:
            break;
        }
    }
};



