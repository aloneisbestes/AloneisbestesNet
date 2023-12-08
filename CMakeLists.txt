# 定义cmake的最低版本
cmake_minimum_required(VERSION 3.0)

# 定义项目名
project("aloneisbestes")

# 设置构建类型为 Debug
set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)

# 根据 CMAKE_BUILD_TYPE 的值设置宏
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG")
else(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif()

# 设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 调用子目录下的cmake
add_subdirectory(src)
add_subdirectory(test)