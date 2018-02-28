#pragma once 

#include <dirent.h>  
#include <sys/stat.h>

#include <string>
#include <memory>

#include "exception/io_exception.h"

namespace bordkv {

//
// notice: using fseeko & ftello for binary stream. 
// https://wiki.sei.cmu.edu/confluence/display/c/FIO19-C.+Do+not+use+fseek%28%29+and+ftell%28%29+to+compute+the+size+of+a+regular+file
// 

inline FILE *fopen_safe(const char *filename, const char *mode) {
    FILE *fp;
    if ((fp = fopen(filename, mode)) == NULL) {
        throw_io_exception("open file to read failed ");
    }
    return fp;
}

inline void fread_safe(void *ptr, size_t size, FILE *stream) {
    if (size == 0) 
        return;
    if (!fread(ptr, size, 1, stream) && !feof(stream)) {
        throw_io_exception("read file failed ");
    }
}

inline void fwrite_safe(const void *buf, size_t size, FILE *fp) {
    if (size == 0) 
        return;
    if (!fwrite(buf, size, 1, fp)) {
        throw_io_exception("write file failed ");
    }
}

inline long ftell_safe(FILE *fp) {
    long offset = ftello(fp);
    if (offset == -1) {
        throw_io_exception("call ftell failed");
    }
    return offset;
}

inline void fseek_safe(FILE *stream, long offset, int whence) {
    if (fseeko(stream, offset, whence) == -1) {
        throw_io_exception("seek to target offset failed");
    }
}

inline DIR *opendir_safe(const char *dirname) {
    DIR *dir = opendir(dirname);
    if (dir == NULL) {
        throw io_exception("call opendir failed");
    }
    return dir;
}

inline void closedir_safe(DIR *dir) {
    if (closedir(dir) == -1) {
        throw_io_exception("close dir failed");
    }
}

inline void stat_safe(const char *name, struct stat *statbuf) {
    if (stat(name, statbuf) == -1) {
        throw_io_exception("get file information failed");
    }
}

inline void must_eof(FILE *fp) {
    if (!feof(fp)) {
        throw_io_exception("read file failed ");
    }
}

typedef std::shared_ptr<FILE> FileHandler;

std::string file_path(const std::string &dir, const std::string &filename);

void ClearAllTemporaryFiles(const std::string &dir);

std::vector<std::string> GetAllFiles(const std::string &dirname);

FileHandler OpenFile(const std::string &filename);


bool ReadFile(FILE *fp, void *buf, size_t size);
std::string ReadFile(FILE *fp, size_t size);
std::string ReadFile(FILE *fp, uint32_t offset, size_t size);
long WriteFile(const void *buf, size_t size, FILE *fp);

} // namespace bordkv
