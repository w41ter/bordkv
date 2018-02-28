#!/bin/bash

mkdir -p build/ && cd build/
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ ../
make 