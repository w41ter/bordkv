ADD_EXECUTABLE(bench kv_bench.cpp)
TARGET_LINK_LIBRARIES(bench kv)

ADD_EXECUTABLE(validate validate.cpp)
TARGET_LINK_LIBRARIES(validate kv)

ADD_EXECUTABLE(file_write_bench benchmark.cpp)