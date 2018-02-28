#pragma once 

#include <functional>

#include "bordkv/type.h"

namespace bordkv {

struct RecordInfo {
    uint32_t size;
    uint32_t key_offset;
    uint32_t value_offset;
};

typedef std::function<void(const Key&, const Value&, const RecordInfo&)> ReadRecordCallback;

void ReadAllRecords(FILE *fp, ReadRecordCallback callback);
RecordInfo WriteRecord(const Key &key, const Value &value, FILE *fp);

} // namespace bordkv
