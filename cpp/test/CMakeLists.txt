# ========================================================
# 测试可执行文件定义
# ========================================================

# 定义一个名为 run_tests 的可执行文件。
# 它包含了测试主入口和各个模块的单元测试代码。
add_executable(run_tests
    main_test.cc         # 测试主程序入口 (包含 main 函数)
    unit_io.cc           # I/O 模块测试用例
    unit_codec.cc        # 压缩/解压模块测试用例
    unit_imgproc.cc      # 图像处理算法测试用例
)

# ========================================================
# 链接依赖
# ========================================================
# 将测试程序链接到我们在根目录定义的 core_lib 静态库。
# PRIVATE 关键字表示这个链接依赖只对 run_tests 自己有效，不会传递给其他目标。
target_link_libraries(run_tests PRIVATE core_lib)

# 为测试注入数据目录与输出目录宏，确保运行时路径正确
target_compile_definitions(run_tests PRIVATE 
    DATA_DIR="${CMAKE_SOURCE_DIR}/../data"
    OUTPUT_DIR="${CMAKE_BINARY_DIR}/test_outputs"
)

# (可选) 如果你使用了 Google Test 等测试框架，需要在这里查找并链接它们。
# 目前假设你是手动写简单的 main 函数进行测试，所以不需要额外的测试框架链接。