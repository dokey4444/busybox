# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (gperftools)

# 编译选项
add_definitions(-std=c++11 -g3 -Wall -fno-omit-frame-pointer)

# 为构建树添加包含路径
include_directories(./gperftools/include)

# 链接目录
link_directories(./gperftools/lib)

# 指定生成目标
add_executable(test main.cpp)
target_link_libraries(test pthread)
target_link_libraries(test tcmalloc)
target_link_libraries(test profiler)

