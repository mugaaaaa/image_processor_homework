cmake_minimum_required(VERSION 3.10)

# 项目名称和版本
project(ImageProcessorCore VERSION 1.0.0 LANGUAGES CXX)

# ========================================================
# 1. 全局设置 (Global Settings)
# ========================================================
# 设置 C++ 标准为 C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 生成 compile_commands.json，方便编辑器插件（如 Clangd）提供智能提示
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 设置构建类型，默认为 Release 以获得更好的性能
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# ========================================================
# 2. 依赖查找 (Dependencies)
# ========================================================
# 查找 OpenCV 包。REQUIRED 表示如果找不到，构建过程将立即停止。
find_package(OpenCV REQUIRED)

# 打印 OpenCV 信息
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

# ========================================================
# 3. 核心库定义 (Core Library)
# ========================================================
# 将 src 目录添加到全局头文件搜索路径。
# 这样在代码中就可以使用 #include "io/ImageIO.h" 这样的相对路径，
# 而不需要写成 #include "../io/ImageIO.h"。
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

# 递归查找 src 目录下所有的源文件 (.cc)
# 注意：在大型长期项目中，显式列出文件通常比 GLOB_RECURSE 更推荐，
# 以避免添加新文件后需要手动重新运行 CMake 的问题。但对于本项目，这样更方便。
file(GLOB_RECURSE CORE_SOURCES
    "src/data_structure/*.cc"
    "src/io/*.cc"
    "src/codec/*.cc"
    "src/imgproc/*.cc"
)

# 创建一个名为 core_lib 的静态库。
# STATIC 意味着它只是一堆目标文件的归档，不会在运行时被动态加载。
add_library(core_lib STATIC ${CORE_SOURCES})

# 为核心库链接 OpenCV 依赖。
# PUBLIC 关键字意味着任何链接了 core_lib 的目标（比如我们的测试程序），
# 也会自动获得 OpenCV 的链接信息和头文件路径。
target_link_libraries(core_lib PUBLIC ${OpenCV_LIBS})

# ========================================================
# 4. 子模块 (Subdirectories)
# ========================================================
# 添加测试子目录。CMake 会进入 test 目录查找并处理那里的 CMakeLists.txt。
add_subdirectory(test)
