cmake_minimum_required(VERSION 3.16)

# 1. 项目名称
project(Ballbattle VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 2. 自动寻找 Qt6 核心组件
# 🎯 ✨【修改位置】：在这里并列加上 Multimedia 模块
find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

# 3. 启用 Qt 特有的元对象编译器
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 4. 【核心：递归搜索所有子文件夹】
file(GLOB_RECURSE SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

# 5. 【核心：拉黑 build 文件夹】从搜索结果中，强行剔除任何带有 "build" 路径的临时文件
list(FILTER SRC_FILES EXCLUDE REGEX ".*/build/.*")

# 6. 编译成可执行程序
add_executable(Ballbattle ${SRC_FILES})

# 7. 将你的程序与 Qt 库链接
# 🎯 ✨【修改位置】：在这里把程序和 Qt6::Multimedia 绑死在一起
target_link_libraries(Ballbattle PRIVATE
    Qt6::Widgets
    Qt6::Multimedia
)