#include "rolling-sum.hpp"

int main(){

    const std::uint32_t windowSize = 30; // seconds
    const std::size_t nbuckets = 10;

    constexpr std::int64_t bucketSize = windowSize * 1000000000 / nbuckets; //nanoseconds
    
    RollingSum<nbuckets, bucketSize> rollingsum;


    return 1;
}

