#include <easylogging++.h>

#include "file.h"
#include "crc32.h"
#include "record.h"
#include "bordkv/type.h"
#include "exception/log_exception.h"

namespace bordkv {

struct RecordHeader {
    uint32_t crc32;
    uint32_t key_size;
    uint32_t value_size;
};

static uint32_t GetCrc32(RecordHeader &header, const Key &key, const Value &value) {
    uint32_t crc = 0;
    crc = crc32(crc, &header.key_size, sizeof(header.key_size));
    crc = crc32(crc, &header.value_size, sizeof(header.value_size));
    crc = crc32(crc, key.c_str(), key.size());
    crc = crc32(crc, value.c_str(), value.size());
    return crc;
}

static size_t FillHeader(RecordHeader &header, const Key &key, const Value &value) {
    header.key_size = static_cast<uint32_t>(key.size());
    header.value_size = static_cast<uint32_t>(value.size());
    header.crc32 = GetCrc32(header, key, value);

    return sizeof(header) + key.size() + value.size();
}

RecordInfo WriteRecord(const Key &key, const Value &value, FILE *fp) {
    RecordInfo info;
    RecordHeader header;

    info.size = FillHeader(header, key, value);

    WriteFile(&header, sizeof(header), fp);
    info.key_offset = WriteFile(key.c_str(), key.size(), fp);
    info.value_offset = WriteFile(value.c_str(), value.size(), fp);

    // LOG(TRACE) << "write record crc32: " << header.crc32 
    //     << " key: `" << key 
    //     << "` s: " << header.key_size
    //     << " value: `" << value
    //     << "` s: " << header.value_size;

    return info;
}

bool ReadRecord(RecordInfo &info, Key &key, Value &value, FILE *fp) {
    RecordHeader header;

    key.clear();
    value.clear();
    info.size = 0;
    info.key_offset = 0;
    info.value_offset = 0;
    if (!ReadFile(fp, &header, sizeof(header))) {
        return false;
    }

    info.size = sizeof(header) + header.key_size + header.value_size;
    
    info.key_offset = ftell_safe(fp);
    key = ReadFile(fp, header.key_size);
    info.value_offset = ftell_safe(fp);
    value = ReadFile(fp, header.value_size);
    
    // LOG(TRACE) << "read record crc32: " << header.crc32 
    //     << " key: `" << key 
    //     << "` s: " << header.key_size
    //     << " value: `" << value
    //     << "` s: " << header.value_size;

    if (GetCrc32(header, key, value) != header.crc32) {
        LOG(TRACE) << "crc32 wrong, want: " << header.crc32 
            << " but get: " << GetCrc32(header, key, value);
        // crc32 验证失败，那么说明存在错误？
        throw log_exception("checksum failed");
    }

    return true;
}

void ReadAllRecords(FILE *fp, ReadRecordCallback callback) {
    RecordInfo info;
    Key key;
    Value value;
    uint32_t count = 0;

    fseek_safe(fp, 0, SEEK_SET);
    while (ReadRecord(info, key, value, fp)) {
        callback(key, value, info);
        count++;
    }

    must_eof(fp);

    LOG(INFO) << "read " << count << " records from file";
}

} // namespace bordkv
