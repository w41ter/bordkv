#include <algorithm>
#include <vector>
#include <string>
#include <chrono>

#include <easylogging++.h>

#include "string_helper.h"
#include "bitcask.h"

namespace bordkv {

namespace {

const std::string db_file_suffix(".kvr");

static std::string GenerateFileName(const std::string &dir, size_t id = 0) {
    char buf[12];
    sprintf(buf, "%010zu", id);
    return file_path(dir, buf + db_file_suffix);
}

static void SortedStableFileByTimestamp(std::vector<std::string> &files) {
    std::sort(files.begin(), files.end(), std::less<std::string>());
}

static std::vector<std::string> FilterDBFiles(const std::vector<std::string> &files) {
    std::vector<std::string> target_files;
    for (auto &name :files) {
        if (ends_with(name, db_file_suffix))
            target_files.push_back(name);
    }
    return target_files;
}

}

struct Bitcask::Write {
    const Key key;
    const Value value;
    std::promise<void> promise;

    Write(Write&& rhs)
        : key{ std::move(rhs.key) }
        , value{ std::move(rhs.value) }
        , promise{ std::move(rhs.promise) }
    {  }

    Write(const Key &k, const Value &v)
        : key {k}
        , value {v}
    {}
};

Bitcask::Bitcask(const std::string &dir, const Option &opts) 
    : opts_{ opts } 
    , dir_{ dir }
    , running_{ true } {
    LOG(INFO) << "open db at " << dir;

    // ClearAllTemporaryFile(dir);
    auto files = FilterDBFiles(GetAllFiles(dir));
    if (files.empty()) {
        CreateActiveFile();
    } else {
        RestoreFiles(files);
    }
    
    commit_worker_thread_ = std::thread(std::bind(&Bitcask::CommitWorker, this));
}

Bitcask::~Bitcask() {
    LOG(INFO) << "close db";

    running_ = false;
    commit_worker_thread_.join();
}

void Bitcask::CreateActiveFile() {
    std::string active_file = GenerateFileName(dir_);
    LOG(INFO) << "current dir is empty, try create active file " << active_file;
    active_file_handler_ = ActiveFile::Create(active_file);
}

void Bitcask::RestoreFiles(std::vector<std::string> &files) {
    SortedStableFileByTimestamp(files);
    std::string active_file = files.back();
    files.pop_back();

    for (auto &filename : files) {
        LOG(INFO) << "find stable file " << filename;
        auto handle = StableFile::Restore(NextFileId(), filename, metrics_, record_map_);
        stable_file_handlers_.emplace_back(handle);
    }

    LOG(INFO) << "find active file " << active_file;
    active_file_handler_ = ActiveFile::Restore(
        NextFileId(), active_file, record_map_, active_map_);
}

std::optional<Value> Bitcask::Get(const Key &key) {
    auto option = record_map_.Get(key);
    if (!option) {
        /* not exists */
        return {};
    }
    
    auto &record = *option;

    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (record.file_id == NextFileId()) {
        lock.unlock();
        auto value = active_map_.Get(key);
        if (value) 
            return value;
        throw std::runtime_error("error, no such key");
    } else {
        auto &stable_file_handler = stable_file_handlers_[record.file_id];
        lock.unlock();
        return stable_file_handler->Read(key, record.offset, record.size);
    }
}

std::future<void> Bitcask::Put(const Key &key, const Value &value) {
    if (!running_) {
        throw std::runtime_error("call Bitcask:: on stopped db");
    }

    std::unique_lock<std::shared_mutex> lock(mutex_);
    write_batch_.emplace_back(Write{ key, value });
    return write_batch_.back().promise.get_future();
}

std::future<void> Bitcask::Delete(const Key &key) {
    if (!running_) {
        throw std::runtime_error("call Bitcask:: on stopped db");
    }

    std::unique_lock<std::shared_mutex> lock(mutex_);

    Value target_value;
    write_batch_.emplace_back(Write{ key, target_value });
    return write_batch_.back().promise.get_future();
}

size_t Bitcask::NextFileId() const {
    return stable_file_handlers_.size();
}

StableFile::Handler Bitcask::Degenerate(ActiveFile::Handler handler) {
    // begin rotate file.
    return std::make_shared<StableFile>(handler->Rotate(), metrics_);
}

void Bitcask::CommitWrites(std::vector<Write> &writes) {
    std::vector<StableFile::Handler> stable_handlers;
    std::vector<HintRecord> wait_commit_records;

    ActiveFile::Handler active_handler = active_file_handler_;
    
    uint32_t offset;
    size_t first_active_records = 0;
    size_t next_file_id = NextFileId(); // no need lock, because only write is itself
    const size_t commit_group_size = writes.size();

    for (size_t i = 0; i < commit_group_size; i++) {
        auto &[key, value, promise] = writes[i];
        offset = active_handler->Write(key, value);
        HintRecord record = HintRecord{ 
            next_file_id, static_cast<uint32_t>(value.size()), offset };
        wait_commit_records.emplace_back(record);
        if (offset + value.size() < opts_.log_file_size) 
            continue;
        
        active_handler->Sync();
        stable_handlers.emplace_back(Degenerate(active_handler));
        std::string new_file_name = GenerateFileName(dir_, ++next_file_id);
        active_handler = ActiveFile::Create(new_file_name);
        first_active_records = i + 1;
    }

    active_handler->Sync();

    // apply wirte batch
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        stable_file_handlers_.insert( 
            stable_file_handlers_.end(), 
            stable_handlers.begin(), 
            stable_handlers.end());
        active_file_handler_ = active_handler;
    }

    if (!stable_handlers.empty()) {
        active_map_.Clear();
    }

    for (size_t i = 0; i < commit_group_size; i++) {
        auto &[key, value, promise] = writes[i];
        auto &record = wait_commit_records[i];
        if (first_active_records <= i) {
            active_map_.Apply(key, value);
        }
        promise.set_value();
        record_map_.Apply(key, record);
    }
}

void Bitcask::CommitWorker() {
    using std::swap;
    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::microseconds;

    std::vector<Write> writes;
    while (true) {
        {
            std::unique_lock<std::shared_mutex> lock(mutex_);
            swap(writes, write_batch_);
        }

        if (!writes.empty()) {
            //  wirte batch to file.
            CommitWrites(writes);
            writes.clear();
        }
        
        if (!running_)
            break;

        std::this_thread::yield();
    }
}

} // namespace bordkv
