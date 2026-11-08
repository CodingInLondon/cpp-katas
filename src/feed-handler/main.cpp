#include "telemetry.hpp"
#include "claude/itch-parser.hpp"
#include "rolling-sum.hpp"
#include "rolling-max.hpp"
#include "pool.hpp"
#include "spsc.hpp"
#include "book.hpp"

#include <thread>
#include <print>

// Holds all aggregate calculators and pass them the tick
class TickHandler{

    public:

    TickHandler(TelemetryCounters& counters):_counters(counters){

    }

    void on_tick(const Tick& tick){

        _ring_buffer.push(tick);

        // Calculate aggregates
        _rolling_sum.on_tick(tick);
        _rolling_max.on_tick(tick);

        // Store aggregates
        _counters.rolling_sum.store(_rolling_sum.get_sum(), std::memory_order_relaxed);
        if (auto max = _rolling_max.get_max())
            _counters.rolling_max.store(*max, std::memory_order_relaxed);
        _counters.last_update_ns.store(tick.ts_ns, std::memory_order_relaxed);


        //To delete
        _book.on_tick(tick);

    }


    private:

    RollingSum<30, 1000000000> _rolling_sum;
    RollingMax<30000000000> _rolling_max;
    RingBuffer<Tick, 65536> _ring_buffer;
    Book<65536, 65536> _book;
    Pool<Tick, 65536> pool;
    

    TelemetryCounters& _counters;
};


int main(){

    TelemetryCounters counters;
    TickHandler handler(counters);

    // Thread 1 (main thread, pinned to isolated core)
    ItchParser<TickHandler&> parser(handler, {
        .group = "239.1.1.1",
        .interface_ip = "10.211.55.5",
        .port = 12345});

    // Thread 2 (pinned to isolated core)
    // BookBuilder book_builder;

    // Thread 3 (not pinned)
    Telemetry telemetry(counters);
    telemetry.start();

    parser.start();
 
    return 0;
}