#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <map>
#include <unordered_map>

#include "cache_statistics.hpp"
#include "math/saturation_arithmetic.h"

class TTLFIFOCache {
    std::uint64_t const ttl_s_ = 1 << 30;
    std::size_t const capacity_;

    std::unordered_map<std::uint64_t, bool> map_;
    std::multimap<std::uint64_t, std::uint64_t> expiration_queue_;
    std::uint64_t logical_time_ = 0;

public:
    static constexpr char name[] = "TTLFIFOCache";
    CacheStatistics statistics_;

    TTLFIFOCache(std::size_t capacity)
        : capacity_(capacity)
    {
    }

    int
    access_item(std::uint64_t const key)
    {
        assert(map_.size() == expiration_queue_.size());
        if (map_.size() >= capacity_) {
            auto const x = expiration_queue_.begin();
            std::uint64_t victim_key = x->second;
            expiration_queue_.erase(x);
            std::size_t i = map_.erase(victim_key);
            assert(i == 1);
            assert(map_.size() + 1 == capacity_);
        }

        if (map_.count(key)) {
            map_[key] = true;
            statistics_.hit();
        } else {
            map_[key] = false;
            uint64_t eviction_time_ms =
                saturation_add(logical_time_,
                               saturation_multiply(1000, ttl_s_));
            expiration_queue_.emplace(eviction_time_ms, key);
            statistics_.miss();
        }

        ++logical_time_;
        return 0;
    }
};
