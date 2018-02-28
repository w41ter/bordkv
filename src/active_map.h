#pragma once 

#include <optional>

#include <libcuckoo/cuckoohash_map.hh>

#include "uncopyable.h"
#include "bordkv/type.h"

namespace bordkv {

class ActiveMap {
    __UNCOPYABLE__(ActiveMap);
public: 
    explicit ActiveMap() = default;
    ~ActiveMap() = default;

    std::optional<Value> Get(const Key &key) const;
    void Apply(const Key &key, const Value &value);
    void Put(const Key &key, const Value &value);
    void Remove(const Key &key);
    void Clear();

private:
    cuckoohash_map<Key, Value> map_;
};

} // namespace bordkv
