#include <cstdint>
#include <cstddef>
#include <new>
#include <iostream>

struct Tick {
    std::uint64_t ts;
    double px;
};


int main(){

    // pre-allocate buffer
    alignas(Tick) std::byte slot[sizeof(Tick)];

    std::uint64_t ts = 1;
    double px = 100.5;

    // Producer thread
    new (slot) Tick{ts, px};

    // Consumer thread
    auto* tick = std::launder(reinterpret_cast<Tick*>(slot));

 }
