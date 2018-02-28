#pragma once 

#include <atomic>

#include <easylogging++.h>

#include "uncopyable.h"

namespace bordkv {

class CacheMetrics {
    __UNCOPYABLE__(CacheMetrics);
public:
    CacheMetrics() = default;
    ~CacheMetrics() {
        uint64_t hit = hit_;
        uint64_t miss = miss_;
        uint64_t total = hit + miss; 

        if (total == 0) 
            total = 1;

        LOG(INFO) << "cache total call: " << total;
        LOG(INFO) << "cache miss call: " << miss; 
        LOG(INFO) << "cache hit/total: " << ((double) hit) / total;
    }

    void Hit() {
        hit_++;
    }

    void Miss() {
        miss_++;
    }

private: 
    std::atomic<uint64_t> hit_ = 0;
    std::atomic<uint64_t> miss_ = 0;
};

} // namespace bordkv
