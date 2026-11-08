#pragma once
#include <cstddef>
#include <cstdint>
#include <map>
#include <unordered_map>
#include <cassert>

#include "tick.hpp"

// There is one book per intrument / venue 
// Each book contains both sides.

// Each book: price levels

// For each price level: 1 list of orders, sorted by arrival time. 

// Access: 
// - by price level
// - by order id (to cancel/modify an order)

// 1. Hash map from order id to Order node.

// 2. std::map to store price levels. One intrusive list per level. Each level points to the tail of the list.


// Intrusive list makes it easy to delete an order looked up by order Id
struct Order{

    std::int64_t order_id;
    std::int64_t price; // in ITCH units
    std::int64_t quantity; // remaining quantity
    bool bid; // side    

    Order* prev{};
    Order* next{};

};

struct Level{

    Order* tail{};// This is the last order in the queue. New orders are inserted here.
    std::int64_t total_qty{};
};


// There is one such book per instrument / venue / side
// To be created on the heap
template <std::size_t max_levels, std::size_t max_orders> 
class Book{

    static_assert(max_levels > 0);
    static_assert(max_orders > 0);


    private:

    // Map order Ids to Orders
    std::unordered_map<std::int64_t, Order> _orders; // start with a unordered_map. Later switch to hashmap

    // Levels. Start with a map. TODO: move to a std::array<> indexed by price ticks and benchmark the difference.
    std::map<std::int64_t, Level, std::greater<std::int64_t>> _bid_levels;
    std::map<std::int64_t, Level> _ask_levels;


    public:

    Book(){
        _orders.reserve(max_orders); // Ensure max_orders can be accomodated without re-hashing.
    }

    void on_tick(const Tick& tick){
        switch (tick.op)
        {
        case Operation::add:{
                assert(tick.order_id > 0);

                if (tick.order_id > 0)[[likely]]{

                    auto [it, inserted] = _orders.try_emplace(tick.order_id);

                    assert(inserted);

                    if (!inserted)[[unlikely]]
                        break;

                    //Order& order = _orders[tick.order_id];//Create the new order in the hash map
                    Order& order = it->second;

                    order.order_id = tick.order_id;
                    order.price = tick.price;
                    order.quantity = tick.qty;
                    order.bid = tick.bid;

                    // Add new level if it did not exist
                    Level& level = tick.bid ? _bid_levels[tick.price] : _ask_levels[tick.price];

                    // Insert at the tail of the list
                    if (level.tail != nullptr){
                        level.tail->next = &order;
                        order.prev = level.tail;
                    }

                    level.tail = &order;
                    level.total_qty += tick.qty;

                }
            }

            break;
        case Operation::del:{

                if (tick.order_id > 0){
                    // find the order

                    auto orders_it = _orders.find(tick.order_id);

                    if (orders_it != _orders.end())[[likely]]{

                        Order& order_to_delete = orders_it->second;

                        if (order_to_delete.bid)
                            unlink_order(_bid_levels, order_to_delete);
                        else
                            unlink_order(_ask_levels, order_to_delete);

                        _orders.erase(orders_it);
                    }
                }
            }
            break;
        
        default:
            break;
        }
    }

    private:

    // Helper function used for delete order
    template <typename T> void unlink_order(T& levels, Order& order_to_delete){

        // Re-link the previous order to the next
        if (order_to_delete.prev != nullptr){
            order_to_delete.prev->next = order_to_delete.next;
        }

        if (order_to_delete.next != nullptr){
            order_to_delete.next->prev = order_to_delete.prev;
        }

        auto levels_it = levels.find(order_to_delete.price);

        assert(levels_it != levels.end());

        if (levels_it != levels.end()){

            Level& level = levels_it->second;   

            level.total_qty -= order_to_delete.quantity;
            assert(level.total_qty >= 0);

            // Check if we're deleting the tail
            if (order_to_delete.next == nullptr){                
                if (order_to_delete.prev == nullptr){
                    // Level is now empty therefore remove it

                    levels.erase(levels_it);
                }
                else{
                    // update the level to the new tail.
                    level.tail = order_to_delete.prev;
                }                        
            }
        }
    }

};



