#pragma once 

#include <future>
#include <memory>
#include <optional>
#include <string>

#include "bordkv/options.h"
#include "bordkv/type.h"

namespace bordkv {

class BordKV {
public:
    static BordKV Create(const std::string &dir, const Option &opts);
    ~BordKV() {}
    
    std::optional<Value> Get(const Key &key);
    std::future<void> Put(const Key &key, const Value &value);
    std::future<void> Delete(const Key &key);

private:
    BordKV() {}

    class Impl;

    std::shared_ptr<Impl> impl_;
};

} // namespace bordkv
