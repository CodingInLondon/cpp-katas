#pragma once
#include "tick.hpp"

#include <deque>
#include <optional>

// Maintain a list of ticks. 
// When a new tick arrives:
// Remove expired ticks from the front. Expired = (now - ts) > window_size_ns
// Remove all ticks with price smaller than new one then push new price at the back
// (because older smaller ticks will never be max )
// -> Max is always at the front of the list
// 
// Maintain ticks in a std::deque (because pop from the front, push/pop at the back )


// Max over a rolling window. 
// 1st version: not bucketed. 
// Worst case scenario for occupancy: window_size_ns * arrival rate
template <std::int64_t window_size_ns>
class RollingMax{

    public :
    void on_tick(const Tick& tick){

        // Drop late ticks
        if (!_slots.empty() && (tick.ts_ns < _slots.back().ts_ns))
            return;

        // 1. evict old ticks
        auto cutoff = tick.ts_ns - window_size_ns;

        while(!_slots.empty() &&
                (_slots.front().ts_ns < cutoff )){
                    _slots.pop_front();
        }

        // 2.  Remove smaller ticks
        while(!_slots.empty() && _slots.back().price <= tick.price){
            _slots.pop_back();
        }

        // 3. Add new tick
        _slots.push_back(tick);

    }

    // Not thread-safe. TODO: make it thread safe
    std::optional<std::int64_t> get_max() const{

        if (_slots.empty())
            return std::nullopt;
        else
            return _slots.front().price;

    }

    private:

    std::deque<Tick> _slots;


};