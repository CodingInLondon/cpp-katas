#pragma once
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <print>
#include <thread>
#include <stop_token>
#include <cstdint>
#include <cassert>

// Shared between telemetry thread and rolling window aggregators
struct alignas(64) TelemetryCounters{
    std::atomic<std::int64_t> last_update_ns{0};
    std::atomic<std::int64_t> rolling_sum{0};
    std::atomic<std::int64_t> rolling_max{0};
};



// TODO: pin this thread to a core separated from the parser's thread
// It should be pinned to a normal core (non isolated)
class Telemetry{

    public:

    explicit Telemetry(TelemetryCounters& counters):_counters(counters){

    }

    Telemetry& operator=(const Telemetry&) = delete;
    Telemetry(const Telemetry&) = delete;

    void start(){
        assert(!_thread.joinable());
        _thread = std::jthread([this](std::stop_token st){ run(st);});

    }

    private:

    void run(std::stop_token stop){

        constexpr auto period = std::chrono::minutes(1);
        std::mutex m;
        std::condition_variable_any cv;
        auto next = std::chrono::steady_clock::now() + period;


        while(!stop.stop_requested()){

            {
                std::unique_lock lock(m);

                cv.wait_until(lock, stop, next, []{ return false;} );
                if (stop.stop_requested()) break;
            }

            report();

            next += period;
            auto now = std::chrono::steady_clock::now();
            while (next <= now) // Increment next in case it is in the past
                next += period;

        }


    }

    void report(){
        // Read counters and write
        auto ts = _counters.last_update_ns.load(std::memory_order_relaxed);
        auto sum = _counters.rolling_sum.load(std::memory_order_relaxed);
        auto max = _counters.rolling_max.load(std::memory_order_relaxed);
        auto wallclock = std::chrono::system_clock::now();
        std::println("{} ts: {} Max: {} Sum: {}", wallclock, ts, max, sum);
    }


    TelemetryCounters& _counters;// shared with the main thread

    std::jthread _thread; //  must be declared last
};

