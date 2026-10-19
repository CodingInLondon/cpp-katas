#include "telemetry.hpp"
#include "claude/itch-parser.hpp"
#include "rolling-sum.hpp"
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

        _rolling_sum.on_tick(tick);
    }

    std::int64_t sum() const{
        return _rolling_sum.get_sum();
    }

    private:

    RollingSum<30, 1000000000> _rolling_sum;
    RingBuffer<Tick, 65536> _ring_buffer;
    Book<65536> _book;

    TelemetryCounters& _counters;
};


int main(){

    TelemetryCounters counters;
    TickHandler handler(counters);

    ItchParser<TickHandler&> parser(handler, {.port = 12345});

    Telemetry telemetry(counters);
    std::thread telemetry_thread(&Telemetry::run, &telemetry);

    parser.start();
    const auto& s = parser.stats();
    std::println("packets {} messages {} ticks {} lost {} malformed {} sum {}",
             s.packets, s.messages, s.ticks, s.lost_messages, s.malformed, handler.sum());    


    telemetry_thread.join();

 
    return 0;
}