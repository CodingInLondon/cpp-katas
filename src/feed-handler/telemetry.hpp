#pragma once
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <print>
#include <thread>
#include <stop_token>
#include <cstdint>

// Shared between telemetry thread and rolling window aggregators
struct alignas(64) TelemetryCounters{
    std::atomic<std::int64_t> rolling_sum{0};
    std::atomic<std::int64_t> rolling_max{0};
};



// TODO: pin this thread to a core separated from the parser's thread
class Telemetry{

    public:

    Telemetry(TelemetryCounters& counters):_counters(counters){

    }

    Telemetry(const Telemetry& ) = delete; // because of jthread
    Telemetry(const Telemetry&& ) = delete;

    void start(){
        _thread = std::jthread([this](std::stop_token st){ run(st);});

    }

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


            // Read counters and write
            auto max = _counters.rolling_max.load();
            auto wallclock = std::chrono::system_clock::now();
            std::println("{} Max: {}", wallclock, max);

            // Sleep till now + 1 min * i
            next += period;
            auto now = std::chrono::steady_clock::now();
            while (next <= now) // Increment next in case it is in the past
                next += period;

        }


    }

    private:

    TelemetryCounters& _counters;

    std::jthread _thread;
};

