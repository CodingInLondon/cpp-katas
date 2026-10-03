#include "rolling-sum.hpp"

int main(){

    const std::int64_t windowSize = 30; // seconds
    const std::size_t nbuckets = 10;

    constexpr std::int64_t bucketSize = windowSize * 1000000000 / nbuckets; //nanoseconds
    
    RollingSum<nbuckets, bucketSize> sum;


    sum.on_tick(Tick{0, 100, 50});
    const auto& s = sum.get_sum();

    return 1;
}

