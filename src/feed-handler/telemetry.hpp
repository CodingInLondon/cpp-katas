#pragma once
#include <atomic>

struct alignas(64) TelemetryCounters{
    std::atomic<std::int64_t> rolling_sum{0};
};



class Telemetry{

    public:

    Telemetry(TelemetryCounters& counters):_counters(counters){

    }

    void run(){


    }

    private:

    TelemetryCounters& _counters;
};

