// A simple circular buffer for market data ticks
// Fixed capacity, no heap allocation after construction
// Support push (add tick) and pop (retrieve oldest tick)
// Ticks should be moved in, not copied

#include <string>
#include <cstdint>
#include <array>
#include <iostream>


struct Tick {
    std::string symbol;
    double price;
    std::uint64_t timestamp;

};

static_assert(std::is_nothrow_move_constructible<Tick>::value,
              "Tick move constructor must be noexcept for vector efficiency");


template<std::size_t N>
class TickBuffer {
    // your implementation
    public:

    TickBuffer() : next(0), tail(0){

    }

    void push(Tick t){

        if ( (next+ 1 ) % N == tail)
            // full
            return;

        buffer[next] = std::move(t);

        next = (next + 1) % N; 

    }


    bool pop(Tick& out){
        if (next == tail){
            // empty
            return false;
        }    

        out = std::move(buffer[tail]); // move since it won't be accessed any more. Left in unspecified state
        tail = (tail+1) % N;

        return true;
    }

    private:

    std::array<Tick, N> buffer;
    std::uint16_t next;// next available empty slot
    std::uint16_t tail;// tail of the buffer
};



int main() {
    // Capacity N = 4  →  3 usable slots (one is always reserved to distinguish
    // full from empty when next == tail).
    TickBuffer<4> buffer;

    // ── pop on empty ─────────────────────────────────────────────────────────
    Tick out;
    std::cout << "pop on empty -> " << std::boolalpha << buffer.pop(out) << '\n';

    // ── fill to capacity ─────────────────────────────────────────────────────
    buffer.push({"AAPL", 189.23, 1'000});
    buffer.push({"MSFT", 412.50, 1'001});
    buffer.push({"GOOG", 142.11, 1'002});

    // ── push on full: silently dropped (push returns void in current impl) ───
    buffer.push({"NVDA", 950.00, 1'003});  // should be rejected — buffer full

    // ── drain ────────────────────────────────────────────────────────────────
    std::cout << "-- drain --\n";
    while (buffer.pop(out))
        std::cout << "  " << out.symbol << " @ " << out.price
                  << "  ts=" << out.timestamp << '\n';

    // ── wrap-around: push/pop past the end of the underlying array ──────────
    buffer.push({"TSLA", 245.67, 2'000});
    buffer.push({"AMD",  162.80, 2'001});
    buffer.pop(out);
    std::cout << "after wrap pop: " << out.symbol << '\n';
    buffer.push({"INTC", 43.10, 2'002});

    std::cout << "-- drain after wrap --\n";
    while (buffer.pop(out))
        std::cout << "  " << out.symbol << " @ " << out.price
                  << "  ts=" << out.timestamp << '\n';

    // ── verify move-in: the source string is left empty after push ──────────
    std::string sym = "MOVED";
    Tick t{std::move(sym), 1.0, 3'000};   // Tick constructed by moving sym
    // NB: 'sym' is now in a valid-but-unspecified state; on libstdc++ it's typically empty
    buffer.push(std::move(t));
    buffer.pop(out);
    std::cout << "moved-in symbol round-trip: " << out.symbol << '\n';

    return 0;
}