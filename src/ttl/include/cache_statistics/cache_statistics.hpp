#pragma once

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

struct CacheStatistics {
    std::uint64_t hits_ = 0;
    std::uint64_t misses_ = 0;
    std::uint64_t total_accesses_ = 0;

    /// @brief  Register a cache hit.
    void
    hit(size_t const size_bytes = 1)
    {
        hits_ += size_bytes;
        total_accesses_ += size_bytes;
    }

    /// @brief  Register a cache miss.
    void
    miss(size_t const size_bytes = 1)
    {
        misses_ += size_bytes;
        total_accesses_ += size_bytes;
    }

    /// @brief  Get the miss ratio.
    /// @returns miss rate on success; -1.0 on failure.
    double
    miss_rate() const
    {
        if (total_accesses_ == 0) {
            return -1.0;
        }
        return (double)misses_ / total_accesses_;
    }

    /// @brief  Pretty print the cache statistics.
    void
    print(std::string const algorithm, std::size_t cache_size) const
    {
        std::string header = "Cache Statistics for '" + algorithm +
                             "' of size " + std::to_string(cache_size);
        std::string underline = std::string(header.size(), '=');
        std::string info =
            "Total Accesses: " + std::to_string(total_accesses_) +
            " | Hits: " + std::to_string(hits_) +
            " | Misses: " + std::to_string(misses_) + " | Miss Ratio: " +
            std::to_string((double)100 * this->miss_rate()) + "%";
        std::cout << header << std::endl;
        std::cout << underline << std::endl;
        std::cout << info << std::endl;
    }
};
