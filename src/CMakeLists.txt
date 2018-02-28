
SET(SOURCE_FILES 
    active_file.cpp
    active_map.cpp
    bitcask.cpp 
    file.cpp
    kv.cpp
    record_map.cpp
    record.cpp
    stable_file.cpp )

ADD_LIBRARY(kv ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(kv -lpthread -lm -leasyloggingpp -lcrc32c)
