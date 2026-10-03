
#include <cstdint>
#include <concepts>
#include <array>
#include <atomic>

struct Tick{
    std::uint64_t price{0};
    std::uint64_t vol{0};
    std::uint64_t ts{0}; //timestamp in nanoseconds.
};


template <std::size_t nbuckets, std::int64_t bucketsizens>
class RollingSum{


    struct Bucket{
        std::uint64_t running_total_vol{0};
    };

    public:

    // Called by feed handler for every tick
    void on_tick(const Tick& tick){

        //Is it a new bucket?
        std::uint64_t current_bucket = tick.ts / bucketsizens;

        if (current_bucket != _last_bucket){
            // This is a new bucket
            // expire all buckets since the last one.
            std::size_t last_bucket_index = _current_bucket_index;
            _current_bucket_index = current_bucket % nbuckets;

            for(auto& i = last_bucket_index+1 ; (i%nbuckets) <= _current_bucket_index; i++){
                expire_bucket(i%nbuckets);
            }

        }

        _buffer[_current_bucket_index].runnint_total_vol += tick.vol;
        _total_vol += tick.vol;
    }

    // read the sum. Called by a telemetry thread
    std::uint64_t get_sum(){

        return _total_vol;

    }

    private:

    void expire_bucket(std::uint64_t i){
        // update running totals 

        auto& bucket = _buffer[i];

        /// Adjust total vol
        _total_vol -= bucket.running_total_vol;

        // reset the bucket
        bucket.runnint_total_vol = 0;
        
    }

    //Final sum shared accross threads
    std::atomic<std::uint64_t> _sum;

    // Total volume accumulator
    std::uint64_t _total_vol;

    std::uint64_t _last_bucket;
    std::size_t _current_bucket_index;

    // Fixed-size array, determines the window size. Window (ns) < nbuckets * bucketsizens
    std::array<Bucket, nbuckets> _buffer;

};
