#include "active_map.h"

#include <stdexcept>

namespace bordkv {

std::optional<Value> ActiveMap::Get(const Key &key) const {
    try {
        return map_.find(key);
    } catch (std::out_of_range &e) {
        return {};
    }
}

void ActiveMap::Apply(const Key &key, const Value &value) {
    if (value.empty()) {
        Remove(key);
    } else {
        Put(key, value);
    }
}

void ActiveMap::Put(const Key &key, const Value &value) {
    map_.insert_or_assign(key, value);
}

void ActiveMap::Remove(const Key &key) {
    map_.erase(key);
}

void ActiveMap::Clear() {
    map_.clear();
}

} // namespace bordkv 
