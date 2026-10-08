#pragma once
#include <cstdint>
#include <array>
#include <atomic>
#include "tick.hpp"



// Accumulate the sum of quantities over a rolling time window
// time window is split in buckets
// Window size (ns) is between (nbuckets-1) * bucketsizens and nbuckets * bucketsizens.
template <std::size_t nbuckets, std::int64_t bucketsizens>
class RollingSum{

    static_assert(nbuckets > 0);
    static_assert(bucketsizens > 0);

    struct Bucket{
        std::int64_t running_total_vol{0};
    };

    public:

    // Called by feed handler for every tick
    void on_tick(const Tick& tick){

        if ( tick.ts_ns < _bucket_start )  // detect late ticks
            return; // drop it

        if ( (tick.ts_ns - _bucket_start) >= bucketsizens) { // Timestamp must be in new bucket
            std::int64_t current_bucket = tick.ts_ns / bucketsizens;    

            // This is a new bucket
            _current_bucket_index = current_bucket%nbuckets;
            _bucket_start = current_bucket*bucketsizens;

            // expire all buckets since the last one.
            if ( (current_bucket - _last_bucket) > static_cast<std::int64_t>(nbuckets)){
                //Long silence

                //Reset the bucket buffer and the total
                _total_vol = 0;
                _buffer.fill({});

            }
            else{

                for(auto i = _last_bucket+1 ; i <= current_bucket ; i++){
                    auto& bucket = _buffer[i%nbuckets]; // wrap around

                    /// Adjust total vol
                    _total_vol -= bucket.running_total_vol;

                    // reset the bucket
                    bucket.running_total_vol = 0;
                }
            }

            _last_bucket = current_bucket;            
        }

        _buffer[_current_bucket_index].running_total_vol += tick.qty;
        _total_vol += tick.qty;

    }

    // read the sum. Called by a telemetry thread
    std::int64_t get_sum() const{

        // TODO: make this thread-safe
        return _total_vol;

    }

    private:

    // Total volume accumulator
    std::int64_t _total_vol{0};

    std::int64_t _last_bucket{0};

    // Cached index of the current bucket
    std::size_t _current_bucket_index{0};

    // Cached bucket start
    std::int64_t _bucket_start{0};

    // Fixed-size array, determines the window size. 
    std::array<Bucket, nbuckets> _buffer;

};
