#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>
#include <array>
#include <cassert>

struct Message{
    std::int64_t timestamp; // in ns
    std::int64_t price; // in min tick size
    std::int64_t qty;
};


// N: number of buckets
// BucketSize: size of a bucket in ns
template <std::size_t N, std::int64_t BucketSize>   
class RunningAveragePrice {

    struct Bucket { 
        __int128 notional = 0; 
        std::int64_t qty = 0; 
    };

    std::array<Bucket, N> buckets_{};

    __int128 notional_ = 0;
    std::int64_t qty_ = 0; 
    std::int64_t last_bucket_seq_ = 0;

public:
    void on_message(const Message& msg) noexcept {

        assert(msg.timestamp >= 0);

        const std::int64_t current_bucket_seq = msg.timestamp / BucketSize;

        if (current_bucket_seq < last_bucket_seq_)// ignore late messages
            return;

        if (current_bucket_seq != last_bucket_seq_) // Entering a new bucket.
            clean_stale_buckets(current_bucket_seq); // Clean the whole list to detect stale buckets and adjust totals

        Bucket& b = buckets_[static_cast<std::size_t>(current_bucket_seq)%N ];

        const __int128 n = static_cast<__int128>(msg.price) * msg.qty;

        // Maintain bucket totals
        b.notional += n;  
        b.qty += msg.qty;

        // Maintain running totals
        notional_  += n;  
        qty_  += msg.qty;
    }

    std::optional<double> running_average() const noexcept {          // division on the read path only
        if (qty_ == 0) 
            return std::nullopt;    

        return static_cast<double>(notional_) / static_cast<double>(qty_);
    }

private:


    void clean_stale_buckets(std::int64_t current_bucket_seq) noexcept{


        if ( (current_bucket_seq - last_bucket_seq_ >= static_cast<std::int64_t>(N)) ){ 
        // Long silence detected, we need to scan the whole array. All buckets are stale 
        // and running totals are invalid
            for (std::size_t i=0 ; i< N; i++){
                Bucket& b = buckets_[i];

                // Reset the bucket
                b.notional = 0;
                b.qty = 0;
            }    

            // Reset the totals
            notional_ = 0;
            qty_ = 0;
        }
        else{
            // We only need to scan from the last known bucket to the current bucket 
            // (all stale)
            for (std::int64_t i= last_bucket_seq_+1; i <= current_bucket_seq; i++){

                Bucket& b = buckets_[static_cast<std::size_t>(i)%N];

                // adjust running totals
                notional_ = notional_ - b.notional;
                qty_ = qty_ - b.qty;

                // Reset the bucket
                b.notional = 0;
                b.qty = 0;
            }
        }

        last_bucket_seq_ = current_bucket_seq;                       
    }


};


