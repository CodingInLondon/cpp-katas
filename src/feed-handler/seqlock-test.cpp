#include "seqlock.hpp"
#include <bits/chrono.h>
#include <ostream>

struct Quote{
    std::uint64_t bid;
    std::uint64_t ask;
    std::uint64_t timestamp;
};


int main(){

    seqlock<Quote> lock;
    
    lock.write(Quote(100, 200, std::chrono::steady_clock::now()));


    auto quote = lock.read();

    std::println(quote.ask);
    std::println(quote.bid);
    std::println(quote.timestamp);

    return 1;
}
