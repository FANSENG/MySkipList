cmake_minimum_required(VERSION 3.16)
project(skiplist)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置 include Path
include_directories(${CMAKE_SOURCE_DIR}/src/include)


set(SRCS
        src/main.cpp
)

add_executable(skiplist ${SRCS})



target_link_libraries(skiplist pthread)