#include <easylogging++.h>

#include "hint.h"
#include "record.h"
#include "stable_file.h"

namespace bordkv {

StableFile::StableFile(FileHandler file, CacheMetrics &metrics) 
    : file_{ file }
    , metrics_{ metrics }
    , cache_{ 128 } {

}

Value StableFile::Read(const Key &key, uint32_t offset, uint32_t size) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (cache_.contains(key)) {
        metrics_.Hit();
        return cache_.lookup(key);
    } 
    metrics_.Miss();
    Value value = ReadFile(file_.get(), offset, size);
    cache_.emplace(key, value);
    return value;
}

StableFile::Handler StableFile::Restore(size_t file_id, 
    const std::string &filename, 
    CacheMetrics &metrics,
    RecordMap &record_map) {
    auto callback = [&record_map, file_id](
        const Key &key, const Value &value, const RecordInfo &info) {
        record_map.Apply(key, HintRecord{ 
            .file_id = file_id,
            .size = static_cast<uint32_t>(value.size()), 
            .offset = info.value_offset, 
        });
    };

    LOG(INFO) << "restore stable file from " << filename;
    
    FileHandler handler = OpenFile(filename);
    ReadAllRecords(handler.get(), callback);
    return std::make_shared<StableFile>(handler, metrics);
}

} // namespace bordkv
