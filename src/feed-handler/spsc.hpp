#pragma once
#include <cstddef>
#include <type_traits>
#include <array>
#include <optional>
#include <atomic>
#include <new>

// To be used by one producer thread and one consumer thread only.
template <typename T, std::size_t buckets> // Capacity is buckets-1
requires std::is_trivially_copyable_v<T>
class RingBuffer{

    // It must be a power of 2
    static_assert(buckets > 1);
    static_assert( (buckets & (buckets-1)) == 0);


    private:

    std::array<T, buckets> _ringbuffer{};

    // TODO: benchmark with and without alignas(64)
    alignas(64) std::atomic<std::size_t> _head{0}; //next slot to be written to  by producer
    alignas(64) std::atomic<std::size_t> _tail{0}; //next slot to be read by consumer


    public:

    // Writer pushes into the ring buffer
    // owns the head
    bool push(const T& payload){

        // TODO: benchmark between seq_cst and relaxed / load/acquire
        // TODO: try caching head/tail
        std::size_t head = _head.load(std::memory_order_relaxed);
        std::size_t next_head = (head + 1 ) % buckets;

        if (next_head == _tail.load(std::memory_order_acquire)) // full
            return false;

        _ringbuffer[head] = payload;
        
        _head.store(next_head, std::memory_order_release);

        return true;

    }

    // Readers pops from the ring buffer
    // owns the tail
    std::optional<T> pop(){

        std::size_t tail = _tail.load(std::memory_order_relaxed) ;

        if (tail == _head.load(std::memory_order_acquire))
            //empty
            return std::nullopt;

        const T payload = _ringbuffer[tail];

        std::size_t next_tail = (tail+1) %  buckets;

        _tail.store(next_tail, std::memory_order_release);

        return payload;

    }

};



