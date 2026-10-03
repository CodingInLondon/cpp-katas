#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>
#include <iostream>
#include <expected>
#include <atomic>


template <typename T, std::size_t N>
class RingBuffer{

    public:

    // Producer
    bool AddElement(const T& element){
        
        // read tail relaxed
        std::size_t currentTail = tail.load(std::memory_order_relaxed);

        std::size_t nextTail = (currentTail+1)%N;

        // read head acquire
        if (next == head.load(std::memory_order_acquire)) // ring buffer is full
            return false;

        ticks[next] = element; // Write to the buffer

        // Publish - write tail release
        tail.store(next, std::memory_order_release);

        return true;
    }

    // Consumer
    bool PopElement(T& element){

        std::size_t nextHead = head.load(std::memory_order_relaxed);

        // read head relaxed
        if (head == tail)
            return false;//empty

        // read buffer
        element = ticks[head];

        // write to head release
        head = (head+1)%N;

        return true;
    }

    private:

    std::array<T, N> ticks;

    std::atomic<std::size_t> head = 0; // Where PopElement will read next
    std::atomic<std::size_t> tail = 0; // Where AddElement will write next

};

struct Tick{
    std::int64_t price;

    char symbol[5];

};



int main(){

    RingBuffer<Tick, 100> buffer;

    buffer.AddElement(Tick{123, "APLE"});
    buffer.AddElement(Tick{123, "APLE"});

    Tick element; 
    if (buffer.PopElement(element))
    {

    }

    return 1;
}



      T
H
0 1 2 3