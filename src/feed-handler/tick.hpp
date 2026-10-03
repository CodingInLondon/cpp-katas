#pragma once
#include <cstdint>

struct Tick {
    std::int64_t ts_ns;   // nanoseconds
    std::int64_t price;   
    std::int64_t qty;
};


