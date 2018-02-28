#include <unistd.h>
#include <stdio.h>

#include <stdexcept>

#include <easylogging++.h>

#include "hint.h"
#include "record.h"
#include "active_file.h"

namespace bordkv {

ActiveFile::ActiveFile(FileHandler file) 
    : file_{ file } {
    fseek_safe(file.get(), 0, SEEK_END);
}

uint32_t ActiveFile::Write(const Key &key, const Value &value) {
    RecordInfo info = WriteRecord(key, value, file_.get());
    return info.value_offset;
}

void ActiveFile::Sync() {
    FILE *fp = file_.get();

    fflush(fp);
    fsync(fileno(fp));
}

ActiveFile::Handler ActiveFile::Create(const std::string &filename) {
    LOG(INFO) << "create active file " << filename;
    
    return std::make_shared<ActiveFile>(OpenFile(filename));
}

ActiveFile::Handler ActiveFile::Restore(
    size_t file_id, const std::string &filename, 
    RecordMap &record_map, ActiveMap &map) {
    auto callback = [&record_map, &map, file_id](
        const Key &key, const Value &value, const RecordInfo &info) {
        map.Apply(key, value);
        record_map.Apply(key, HintRecord{ 
            .file_id = file_id,
            .size = static_cast<uint32_t>(value.size()), 
            .offset = info.value_offset, 
        });
    };

    LOG(INFO) << "restore active file from " << filename;
    
    FileHandler handler = OpenFile(filename);
    ReadAllRecords(handler.get(), callback);
    return std::make_shared<ActiveFile>(handler);
}

FileHandler ActiveFile::Rotate() {
    FileHandler handler;
    std::swap(handler, file_);
    return handler;
}

} // namespace bordkv
