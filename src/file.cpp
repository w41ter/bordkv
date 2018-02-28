#include <easylogging++.h>

#include "file.h"
#include "exception/io_exception.h"

namespace bordkv {

std::string file_path(const std::string &dir, const std::string &filename) {
    if (dir.empty() || dir.back() == '/') {
        return dir + filename;
    } else {
        return dir + '/' + filename;
    }
}

void ClearAllTemporaryFiles(const std::string &dir) {

}

std::vector<std::string> GetAllFiles(const std::string &dirname) {
    struct dirent *ptr;
    struct stat statbuf;

    DIR *dir = opendir_safe(dirname.c_str());

    std::vector<std::string> files;
    while ((ptr = readdir(dir)) != NULL) {  
        //跳过'.'和'..'两个目录  
        if (ptr->d_name[0] == '.')  
            continue;
        std::string filename = file_path(dirname, ptr->d_name);
        stat_safe(filename.c_str(), &statbuf);

        if (S_ISREG(statbuf.st_mode)) {
            files.push_back(filename);
        }
    }

    closedir_safe(dir);
    return files;
}


bool ReadFile(FILE *fp, void *buf, size_t size) {
    if (size == 0) {
        return true;
    }

    fread_safe(buf, size, fp);
    if (feof(fp)) 
        return false;
    
    return true;
}

std::string ReadFile(FILE *fp, size_t size) {
    std::string contents(size, '\0');
    fread_safe(&contents[0], size, fp);
    return contents;
}

std::string ReadFile(FILE *fp, uint32_t offset, size_t size) {
    std::string contents(size, '\0');
    fseek_safe(fp, offset, SEEK_SET);
    fread_safe(&contents[0], size, fp);
    return contents;
}

long WriteFile(const void *buf, size_t size, FILE *fp) {
    long offset = ftell_safe(fp);
    if (size != 0) {
        fwrite_safe(buf, size, fp);
    }
    return offset;
}

FileHandler OpenFile(const std::string &filename) {
    LOG(INFO) << "open file " << filename;

    FILE *fp = fopen_safe(filename.c_str(), "ab+");

    return FileHandler(fp, std::fclose);
}

} // namespace bordkv
