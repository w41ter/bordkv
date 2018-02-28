#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <string>

#include <easylogging++.h>

#include "bordkv/kv.h"

using namespace bordkv;

INITIALIZE_EASYLOGGINGPP

void test_put(BordKV kv, 
    const std::vector<std::string> &keys, 
    const std::vector<std::string> &values) {
    for (int i = 0; i < keys.size(); i++) {
        auto &key = keys[i];
        auto &value = values[i];
        kv.Put(key, value).get();
    }
}

void test_get(BordKV kv, 
    const std::vector<std::string> &keys, 
    const std::vector<std::string> &values) {
    for (int i = 0; i < keys.size(); i++) {
        auto &key = keys[i];
        auto &value = values[i];
        auto option = kv.Get(key);
        auto target = option.value_or("empty");
        if (value != target) {
            std::cout << "#" << i+1 << " "
                << "key: `" << key << "` "
                << "want: `" << value << "` "
                << "get: `" << target << "`" << std::endl;
        }
    }
}

int main(int argc,char* argv[]) {
    std::string dir = "./tmp/";
    std::vector<std::string> keys;
    std::vector<std::string> values;

    Option option { 1024 };
    
    srand(time(NULL));

    for (size_t i = 0; i < 10; i++) {
        char buf[32];
        int r = rand();
        sprintf(buf, "key-%d", r);
        keys.emplace_back(buf);
        sprintf(buf, "value-%d", r);
        values.emplace_back(buf);
    }

    {
        BordKV kv = BordKV::Create(dir, option);
        test_put(kv, keys, values);
    }

    {
        BordKV kv = BordKV::Create(dir, option);
        test_get(kv, keys, values);
    }

    remove(dir.c_str());
    
    return 0;
}