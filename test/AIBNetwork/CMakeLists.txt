# 指定头文件路径，PROJECT_SOURCE_DIR: 当前项目的根目录
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/include/AIBNetwork
    ${PROJECT_SOURCE_DIR}/include/AIBBase
    )

# 生成可执行文件
add_executable(tcpserver tcpserver.cpp)

# 外部程序库路径
link_directories(${PROJECT_SOURCE_DIR}/lib)
# 添加外部链接库
target_link_libraries(tcpserver aibnetwork)