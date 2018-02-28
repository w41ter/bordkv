#pragma once 

#include <string>
#include <memory>

#include "file.h"
#include "active_map.h"
#include "record_map.h"
#include "uncopyable.h"
#include "bordkv/type.h"

namespace bordkv {

//
// ActiveFile
//
class ActiveFile {
    __UNCOPYABLE__(ActiveFile);
public:
    typedef std::shared_ptr<ActiveFile> Handler;

    static Handler Create(const std::string &filename);
    static Handler Restore(size_t file_id, 
        const std::string &filename, 
        RecordMap &record_map, ActiveMap &map);

    explicit ActiveFile(FileHandler file);
    ~ActiveFile() = default;

    uint32_t Write(const Key &key, const Value &value);
    FileHandler Rotate();

    void Sync();
private:

    FileHandler file_;
};

} // namespace bordkv
