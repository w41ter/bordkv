#include <string>
#include <thread>
#include <vector>
#include <stdexcept>
#include <iostream>
#include <chrono>

#include <easylogging++.h>

#include "bordkv/kv.h"

using namespace std::chrono;
using namespace bordkv;

const int thread_nums = 32;
const int run_times = 100;

INITIALIZE_EASYLOGGINGPP

void bench_put(BordKV kv, 
    const std::vector<std::string> &keys, 
    const std::vector<std::string> &values) {
    auto begin = high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < thread_nums; i++) {
        threads.emplace_back(std::thread([&kv, &keys, &values]() {
            try {       
                for (int j = 0; j < run_times; j++) {
                    size_t idx = j % keys.size();
                    kv.Put(keys[idx], values[idx]).get();
                }
            } catch (std::exception &e) {
                std::cout << "exception: " << e.what() << std::endl;
            }
        }));
    }
    for (auto &thread : threads) {
        thread.join();
    }

    auto end = high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    double cost = duration.count() / 1000000.f;
    std::cout << "put cost: " << std::fixed << cost << " sec"
        << " and " << (run_times / cost) << " io/s per thread" 
        << std::endl;
}

void bench_get(BordKV kv, 
    const std::vector<std::string> &keys,
    const std::vector<std::string> &values) {
    auto begin = high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < thread_nums; i++) {
        threads.emplace_back(std::thread([&kv, &keys, &values]() {
            try {
                for (int j = 0; j < run_times; j++) {
                    size_t idx = j % keys.size();
                    auto option = kv.Get(keys[idx]);
                    auto &value = values[idx];
                    auto target = option.value_or("empty");
                    if (value != target) {
                        std::cout << "#" << j+1 << " "
                            << "key: `" << keys[idx] << "` "
                            << "want: `" << value << "` "
                            << "get: `" << target << "`" << std::endl;
                    }
                }
            } catch (std::exception &e) {
                std::cout << "exception: " << e.what() << std::endl;
            }
        }));
    }
    for (auto &thread : threads) {
        thread.join();
    }
    auto end = high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    double cost = duration.count() / 1000000.f;
    std::cout << "get cost: " << std::fixed << cost << " sec"
        << " and " << (run_times / cost) << " io/s per thread" 
        << std::endl;
}

void bench_delete(BordKV kv, const std::vector<std::string> &keys) {
    auto begin = high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < thread_nums; i++) {
        threads.emplace_back(std::thread([&kv, &keys]() {
            try {       
                for (int j = 0; j < run_times; j++) {
                    size_t idx = j % keys.size();
                    kv.Delete(keys[idx]).get();
                }
            } catch (std::exception &e) {
                std::cout << "exception: " << e.what() << std::endl;
            }
        }));
    }
    for (auto &thread : threads) {
        thread.join();
    }

    auto end = high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    double cost = duration.count() / 1000000.f;
    std::cout << "delete cost: " << std::fixed << cost << " sec"
        << " and " << (run_times / cost) << " io/s per thread" 
        << std::endl;
}

void bench_tp(BordKV kv, 
    const std::vector<std::string> &keys,
    const std::vector<std::string> &values) {
    auto begin = high_resolution_clock::now();

    std::vector<std::thread> threads;
    for (int i = 0; i < thread_nums; i++) {
        threads.emplace_back(std::thread([&kv, &keys, &values]() {
            try {
                for (int j = 0; j < run_times; j++) {
                    if ((rand() % 10) >= 2) {
                        size_t idx = j % keys.size();
                        auto option = kv.Get(keys[idx]);
                        auto &value = values[idx];
                        auto target = option.value_or("empty");
                        if (value != target) {
                            std::cout << "#" << j+1 << " "
                                << "key: `" << keys[idx] << "` "
                                << "want: `" << value << "` "
                                << "get: `" << target << "`" << std::endl;
                        }
                    } else {
                        size_t idx = j % keys.size();
                        kv.Put(keys[idx], values[idx]).get();
                    }
                }
            } catch (std::exception &e) {
                std::cout << "exception: " << e.what() << std::endl;
            }
        }));
    }
    for (auto &thread : threads) {
        thread.join();
    }
    auto end = high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    double cost = duration.count() / 1000000.f;
    std::cout << "2w/8r cost: " << std::fixed << cost << " sec"
        << " and " << (run_times / cost) << " io/s per thread" 
        << std::endl;
}

char buf[10006];

int main() {
    std::string dir = "./tmp/";
    std::vector<std::string> keys = {"a", "b", "c", "d", "e", "f"};
    std::vector<std::string> values = {"1", "2", "3", "4", "5", "6"};

    Option option { 
        .log_file_size = 1024 * 1024 * 64,
        .cache_size = 1024
    };
    
    srand(time(NULL));

    for (size_t i = 0; i < 100; i++) {
        int r = rand();
        sprintf(buf, "key-%d", r);
        keys.emplace_back(buf);
        sprintf(buf, "value-%d", r);
        values.emplace_back(buf);
    }

    {
        BordKV kv = BordKV::Create(dir, option);
        bench_put(kv, keys, values);
    }

    {
        BordKV kv = BordKV::Create(dir, option);
        bench_get(kv, keys, values);
    }

    {
        BordKV kv = BordKV::Create(dir, option);
        bench_delete(kv, keys);    
    }

    // bench big write 
    keys.clear();
    values.clear();
    for (int i = 0; i < 100; i++) {
        int r = rand();
        sprintf(buf, "key-%d", r);
        keys.emplace_back(buf);

        for (int j = 0; j < 10000; j++) {
            char c = 'a' + (rand() % 26);
            buf[j] = c;
        }
        buf[10000] = '\0';
        values.emplace_back(buf);
    }

    {
        BordKV kv = BordKV::Create(dir, option);
        bench_put(kv, keys, values);
    }

    // bench online transaction process
    {
        BordKV kv = BordKV::Create(dir, option);
        bench_tp(kv, keys, values);
    }

    return 0;
}