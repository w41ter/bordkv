#pragma once 

#include <vector>
#include <shared_mutex> 
#include <unordered_map>

template <typename Key, typename Value>
class locked_map {
    std::shared_mutex mutex_;
    std::unordered_map<Key, Value> map_;

public:
    void insert_or_assign(const Key &key, const Value &value) {
        std::lock_guard lock(mutex_);
        map_.insert_or_assign(key, value);
    }

    std::optional<Value> get(const Key &key) const {
        std::shared_lock lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end())
            return it->second;
        return {};
    }

    bool erase(const Key &key) {
        std::lock_guard lock(mutex_);
        return map_.erase(key);
    }

    void clear() {
        std::lock_guard<std::shared_mutex> lock(mutex_);
        map_.clear();
    }
};

template <typename Key, typename Value>
class sharding_map {
    const size_t mask_;
    std::vector<locked_map<Key, Value>> shards_;
    std::hash<Key> hash_fn_;

    locked_map<Key, Value> & get_shard(const Key &key) {
        auto h = hash_fn_(key);
        return shards_[h & mask_];
    }

    locked_map<Key, Value> & get_shard(const Key &key) const {
        auto h = hash_fn_(key);
        return shards_[h & mask_];
    }

public:
    sharding_map(size_t num_shard = 128)
        : mask_(num_shard-1)
        , shards_(num_shard) {
        if ((num_shard & mask_) != 0)
            throw std::runtime_error("num_shard must be a power of two");
    }

    void insert_or_assign(const Key &key, const Value & value) {
        get_shard(key).insert_or_assign(key, value);
    }

    std::optional<Value> get(const Key &key) const {
        return get_shard(key).get(key);
    }

    bool erase(const Key &key) {
        return get_shard(key).erase(key);
    }

    void clear() {
        /* fixme */
    }
};