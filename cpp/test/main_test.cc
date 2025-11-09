// 简易测试主程序：依次调用各单元与集成测试函数
#include <iostream>
// 创建输出目录
#include <filesystem>
// 使用 std::string 组合测试数据路径
#include <string>
// 为了集成测试：从 PNG/PPM 读取 -> 灰度 -> 缩放 -> 压缩 -> 解压 -> 保存结果
#include "../src/io/ImageIO.h"
#include "../src/imgproc/ImageProcessor.h"
#include "../src/codec/Compressor.h"

int test_io();
int test_codec();
int test_imgproc();

static int test_integration() {
    int failed = 0;
    // 读取 PPM 彩色
    cv::Mat color = ImageIO::loadPpm(std::string(DATA_DIR) + "/color-block.ppm");
    if (color.empty()) { std::cerr << "[IT] load color failed" << std::endl; return ++failed; }
    // 转灰度
    cv::Mat gray = Processor::ToGray(color);
    if (gray.empty()) { std::cerr << "[IT] ToGray failed" << std::endl; ++failed; }
    // 缩放到 256x256（如果不是则尝试缩放）
    cv::Mat resized = Processor::Resize(gray, 256, 256);
    if (resized.empty() || resized.rows != 256 || resized.cols != 256) { std::cerr << "[IT] Resize failed" << std::endl; ++failed; }
    // 压缩保存与解压
    const std::string trip_path = "/tmp/it_color_gray_256.trip";
    if (!Compressor::Save(trip_path, resized)) { std::cerr << "[IT] Save trip failed" << std::endl; ++failed; }
    cv::Mat recon = Compressor::Load(trip_path);
    if (recon.empty() || recon.size() != resized.size() || recon.type() != resized.type()) { std::cerr << "[IT] Load trip mismatch" << std::endl; ++failed; }
    // 保存 PNG 输出
    if (!ImageIO::savePng("/tmp/it_output.png", recon)) { std::cerr << "[IT] save png failed" << std::endl; ++failed; }
    return failed;
}

int main() {
    int failed = 0;
    // 准备输出目录
    try { std::filesystem::create_directories(OUTPUT_DIR); } catch (...) {}
    std::cout << "[RUN] IO tests..." << std::endl;
    failed += test_io();
    std::cout << "[RUN] Codec tests..." << std::endl;
    failed += test_codec();
    std::cout << "[RUN] ImgProc tests..." << std::endl;
    failed += test_imgproc();
    std::cout << "[RUN] Integration test..." << std::endl;
    failed += test_integration();
    if (failed == 0) {
        std::cout << "[OK] All tests passed." << std::endl;
    } else {
        std::cout << "[FAIL] " << failed << " test suite(s) failed." << std::endl;
    }
    return failed == 0 ? 0 : 1;
}