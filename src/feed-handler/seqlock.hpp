
#include <cstdint>
#include <type_traits>
#include <atomic>



// T must be trivially copyable, T is a POD
template <typename T> 
requires std::is_trivially_copyable_v<T>
class seqlock{

    public:
    void write(const T& quote){

        // Increment sequence (to odd)
        _sequence.fetch_add(1, std::memory_order_relaxed);

        // Write
        _quote = quote;

        // Increment sequence
        _sequence.fetch_add(1, std::memory_order_release);

    }


    T read(){

        std::uint64_t sequence1{0}, sequence2{0};
        T quote{0};

        // busy spin 

        do{
            sequence1 = _sequence.load(std::memory_order_acquire);// acquire to view the latest quote

            // read
            quote = _quote;

            sequence2 = _sequence.load(std::memory_order_relaxed);

        }while(sequence1 & 1 || (sequence1 != sequence2)); // sequence is odd or sequence has changed

        return quote;
    }

    private:

    std::atomic<std::uint64_t> _sequence{0}; //shared across threads
    T _quote{0}; // shared across threads, will be flagged as data race


};