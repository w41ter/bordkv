PROJECT(bordkv)

CMAKE_MINIMUM_REQUIRED(VERSION 3.5) 

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -stdlib=libc++")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -g -Werror -fsanitize=undefined")

INCLUDE_DIRECTORIES(third_party/include/)
LINK_DIRECTORIES(third_party/lib)

INCLUDE_DIRECTORIES(include/)
ADD_SUBDIRECTORY(src/)
ADD_SUBDIRECTORY(test/)
