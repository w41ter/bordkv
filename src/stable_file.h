#pragma once 

#include <memory>
#include <mutex>

#include "uncopyable.h"
#include "file.h"
#include "cache.h"
#include "record_map.h"
#include "bordkv/type.h"
#include "cache_metrics.h"

namespace bordkv {

class StableFile {
    __UNCOPYABLE__(StableFile);

public:
    typedef std::shared_ptr<StableFile> Handler;

    static Handler Restore(size_t file_id, 
        const std::string &filename, 
        CacheMetrics &metrics,
        RecordMap &record_map);

    explicit StableFile(FileHandler file, CacheMetrics &metrics);
    ~StableFile() = default;

    Value Read(const Key &key, uint32_t offset, uint32_t size);

private:
    mutable std::mutex mutex_;
    FileHandler file_;
    CacheMetrics &metrics_;
    Cache<Key, Value> cache_;
};

} // namespace bordkv 
