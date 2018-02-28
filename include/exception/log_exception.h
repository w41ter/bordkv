#pragma once 

#include <stdexcept>

class log_exception : public std::runtime_error {
public:
    explicit log_exception(const std::string & arg) 
        : std::runtime_error(arg) {}

    explicit log_exception(const char * arg) 
        : std::runtime_error(arg) {}

    virtual ~log_exception() {}
};
