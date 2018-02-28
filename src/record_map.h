#pragma once 

#include <optional>

#include <libcuckoo/cuckoohash_map.hh>

#include "uncopyable.h"
#include "bordkv/type.h"
#include "hint.h"
namespace bordkv {

class RecordMap {
    __UNCOPYABLE__(RecordMap);
public: 
    explicit RecordMap() = default;
    ~RecordMap() = default;

    std::optional<HintRecord> Get(const Key &key) const;
    void Apply(const Key &key, const HintRecord &value);
    void Put(const Key &key, const HintRecord &value);
    void Remove(const Key &key);
    void Clear();

private:
    cuckoohash_map<Key, HintRecord> map_;
};

} // namespace bordkv
