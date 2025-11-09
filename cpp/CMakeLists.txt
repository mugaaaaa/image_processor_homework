cmake_minimum_required(VERSION 3.10)
project(ImageProcessorCore VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ========================================================
# 依赖查找
# ========================================================
find_package(OpenCV REQUIRED)

# ========================================================
# 核心库定义
# ========================================================
# 包含路径：允许代码中使用 #include "data_structure/Triplet.h"
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# 收集所有源文件
file(GLOB_RECURSE CORE_SOURCES
    "src/data_structure/*.cc"
    "src/io/*.cc"
    "src/codec/*.cc"
    "src/imgproc/*.cc"
)

# 建立静态库
add_library(core_lib STATIC ${CORE_SOURCES})
target_link_libraries(core_lib PUBLIC ${OpenCV_LIBS})

# ========================================================
# 测试子模块
# ========================================================
add_subdirectory(test)