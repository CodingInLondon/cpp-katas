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

        //Is it a new bucket?
        std::int64_t current_bucket = tick.ts_ns / bucketsizens;

        if (current_bucket < _last_bucket) // late tick - drop it
            return; // we will lose this tick's contribution in the sum

        if (current_bucket != _last_bucket){
            // This is a new bucket
            // expire all buckets since the last one.

            if ( (current_bucket - _last_bucket) > static_cast<std::int64_t>(nbuckets)){
                //Long silence

                //Reset the bucket buffer and the total
                _total_vol = 0;
                // for(std::size_t i = 0 ; i< nbuckets; ++i){
                //     _buffer[i].running_total_vol = 0;
                // }
                _buffer.fill({});

            }
            else{

                // std::size_t current_bucket_index = current_bucket % nbuckets;

                for(auto i = _last_bucket+1 ; i <= current_bucket ; i++){
                    auto& bucket = _buffer[i%nbuckets]; // wrap around

                    /// Adjust total vol
                    _total_vol -= bucket.running_total_vol;

                    // reset the bucket
                    bucket.running_total_vol = 0;
                }
            }
        }

        _buffer[current_bucket%nbuckets].running_total_vol += tick.qty;
        _total_vol += tick.qty;

        _last_bucket = current_bucket;
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

    // Fixed-size array, determines the window size. Window (ns) < nbuckets * bucketsizens
    std::array<Bucket, nbuckets> _buffer;

};
