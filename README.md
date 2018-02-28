# bordkv

bordkv is another bitcask like kv storage in C++17.

usage:

```c++
#include <string>

#include <easylogging++.h>

#include "bordkv/kv.h"

using namespace bordkv;

INITIALIZE_EASYLOGGINGPP

int main(int argc,char* argv[]) {
    std::string dir = "./tmp/";
    Option option { 1024 };
    
    BordKV kv = BordKV::Create(dir, option);
    auto option = kv.Get("key");
    auto future = kv.Write("key", "value");
    auto future1 = kv.Delete("key");

    return 0;
}
```