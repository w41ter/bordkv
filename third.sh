#!/bin/bash

export CC=clang 
export CXX=clang++
export CFLAGS=""
export CXXFLAGS="-stdlib=libc++ -O3" 
export LDFLAGS="-stdlib=libc++"

CMAKE_CXX_OPT=""

GCRC32_URL='https://github.com/google/crc32c.git'
GCRC32_PATH='crc32c'
GCRC32_BUILD_OPT="${CMAKE_CXX_OPT} -DCRC32C_BUILD_TESTS=0 -DCRC32C_BUILD_BENCHMARKS=0 -DCRC32C_USE_GLOG=0"

EASYLOG_URL='https://github.com/muflihun/easyloggingpp.git'
EASYLOG_PATH=log
EASYLOG_BUILD_OPT="${CMAKE_CXX_OPT} -Dbuild_static_lib=ON"

LRU_CACHE_URL='https://github.com/goldsborough/lru-cache.git'
LRU_CACHE_PATH=lru 
LRU_CACHE_BUILD_OPT="${CMAKE_CXX_OPT} -DBUILD_LRU_CACHE_TESTS=0"

LIBCUCKOO_URL='https://github.com/efficient/libcuckoo.git'
LIBCUCKOO_PATH=libcuckoo
LIBCUCKOO_BUILD_OPT=""

set -e 

source dep.sh

function init_deps() {
    mkdir -p third_party && cd third_party/

    echo "make_dep ${LIBCUCKOO_PATH} ${LIBCUCKOO_BUILD_OPT}"
    make_dep ${LIBCUCKOO_URL} ${LIBCUCKOO_PATH} ${LIBCUCKOO_BUILD_OPT}

    echo "make_dep ${GCRC32_PATH} ${GCRC32_BUILD_OPT}"
    make_dep_with_submodule ${GCRC32_URL} ${GCRC32_PATH} ${GCRC32_BUILD_OPT}

    echo "make_dep ${EASYLOG_PATH} ${EASYLOG_BUILD_OPT}"
    make_dep ${EASYLOG_URL} ${EASYLOG_PATH} "${EASYLOG_BUILD_OPT}"
}

function clear() {
    echo 'clean...'
    mkdir -p third_party && cd third_party/
    clear_build_history
}

case $1 in 
    clear)
        clear 
        ;;
    *) 
        init_deps 
        ;; 
esac