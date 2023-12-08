cmake_minimum_required(VERSION 3.0)

# 指定lib库输出路径
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# PROJECT_SOURCE_DIR: 当前项目的根目录
# 这里是导入.cpp需要的头文件所在的路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/AIBBase)
include_directories(${PROJECT_SOURCE_DIR}/include/AIBNetwork)

file(GLOB NET_SOURCE "*.cpp")

# 创建库文件   SHARED: 动态库.os, STATIC: 静态库.a
add_library(aibnetwork SHARED ${NET_SOURCE})
# add_library(aibnetwork STATIC ${NET_SOURCE})