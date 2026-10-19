#pragma once
#include <cstddef>
#include <type_traits>
#include <array>

// template <typename T>
// concept PodLike = std::is_trivial_v<T> && std::is_standard_layout_v<T>;

template <typename T, std::size_t buckets>
requires std::is_trivial_v<T>
class RingBuffer{

    // It must be a power of 2
    static_assert( (buckets & (buckets-1)) == 0);


    private:

    std::array<T, buckets> _ringbuffer{};

    alignas(std::hardware_destructive_interference_size) std::atomic<std::size_t> _head{0}; //next slot to be written to  by producer
    alignas(std::hardware_destructive_interference_size) std::atomic<std::size_t> _tail{0}; //next slot to be read by consumer


    public:

    // Writer pushes into the ring buffer
    // owns the head
    bool push(const T& payload){

        std::size_t next_head = (_head.load(std::memory_order_relaxed) + 1 ) % buckets;

        if (next_head == _tail.load(std::memory_order_acquire)) // full
            return false;

        _ringbuffer[next_head] = payload;
        
        _head.store(next_head, std::memory_order_release);

    }

    // Readers pops from the ring buffer
    // ows the tail
    std::expected<T, bool> pop(){

        std::size_t tail = _tail.load(std::memory_order_relaxed) ;

        if (tail == _head.load(std::memory_order_acquire))
            //empty
            return false;

        const T& payload = _ringbuffer[tail];

        std::size_t next_tail = (tail+1) %  buckets;

        _tail.store(next_tail, std::memory_order_release);

    }


};



