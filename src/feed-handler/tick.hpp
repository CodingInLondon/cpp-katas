#pragma once
#include <cstdint>

enum class Operation{
    add,
    partial_cancel,
    del,
    execute,
    replace,
};


struct Tick {
    std::int64_t ts_ns;   // nanoseconds
    std::int64_t price;   
    std::int64_t qty;

    Operation op;

};


