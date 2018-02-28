#include <cstdio>
#include <unistd.h>

#include <atomic>
#include <vector> 
#include <unordered_map>
#include <mutex>
#include <shared_mutex>

#include "uncopyable.h"
#include "bordkv/kv.h"
#include "bitcask.h"

namespace bordkv {

class BordKV::Impl : public Bitcask {
    __UNCOPYABLE__(Impl);
public:
    using Bitcask::Bitcask;
};

BordKV BordKV::Create(const std::string &dir, const Option &opts) {
    BordKV kv;
    kv.impl_ = std::make_shared<BordKV::Impl>(dir, opts);
    return kv;
}

std::optional<Value> BordKV::Get(const Key &key) {
    return impl_->Get(key);
}

std::future<void> BordKV::Put(const Key &key, const Value &value) {
    return impl_->Put(key, value);
}
 
std::future<void> BordKV::Delete(const Key &key) {
    return impl_->Delete(key);
}

} // namespace bordkv
