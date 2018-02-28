#pragma once 

#include <map>
#include <mutex>
#include <optional>
#include <future>
#include <thread> 
#include <atomic>
#include <vector>
#include <shared_mutex>

#include "hint.h"
#include "active_map.h"
#include "record_map.h"
#include "active_file.h"
#include "stable_file.h"
#include "uncopyable.h"
#include "cache_metrics.h"
#include "bordkv/options.h"

namespace bordkv {

class Bitcask {
    __UNCOPYABLE__(Bitcask);
public:
    Bitcask(const std::string &dir, const Option &opts);
    ~Bitcask();

    std::optional<Value> Get(const Key &key);
    std::future<void> Put(const Key &key, const Value &value);
    std::future<void> Delete(const Key &key);

private:
    struct Write;

    void RestoreFiles(std::vector<std::string> &files);
    void CreateActiveFile();

    void CommitWrites(std::vector<Write> &writes);
    void CommitWorker();

    StableFile::Handler Degenerate(ActiveFile::Handler handler);

    size_t NextFileId() const;

    const Option opts_;
    const std::string dir_;

    CacheMetrics metrics_;

    ActiveMap active_map_;
    RecordMap record_map_;

    std::atomic<bool> running_;
    std::thread commit_worker_thread_;
    
    std::shared_mutex mutex_;
    std::vector<Write> write_batch_;
    ActiveFile::Handler active_file_handler_;
    std::vector<StableFile::Handler> stable_file_handlers_;
};

} // namespace bordkv 
