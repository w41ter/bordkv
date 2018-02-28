#pragma once 

#define __UNCOPYABLE__(clazz)           \
clazz(const clazz &) = delete;          \
clazz & operator = (const clazz &) = delete 

