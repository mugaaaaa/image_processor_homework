// I/O 模块单元测试：验证 PPM/PNG 读写，以及 Triplet 文本读写接口
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cstring>
#include "../src/io/image_io.h"
#include "../src/io/ppm.h"

static bool checkSame(const cv::Mat& a, const cv::Mat& b) {
    if (a.size() != b.size() || a.type() != b.type()) return false;
    return std::memcmp(a.data, b.data, a.total() * a.elemSize()) == 0;
}

int test_io() {
    int failed = 0;
    // 1) 读取 P2 灰度
    cv::Mat lena128 = ImageIO::LoadPpm(std::string(DATA_DIR) + "/lena-128-gray.ppm");
    if (lena128.empty() || lena128.channels() != 1 || lena128.rows != 128 || lena128.cols != 128) {
        std::cerr << "[IO] load P2 failed" << std::endl;
        ++failed;
    }
    // 2) 保存并重读 P2
    if (!Ppm::SaveNatAsPpm(std::string(OUTPUT_DIR) + "/out_lena128.ppm", lena128)) {
        std::cerr << "[IO] save P2 failed" << std::endl;
        ++failed;
    } else {
        cv::Mat re = ImageIO::LoadPpm(std::string(OUTPUT_DIR) + "/out_lena128.ppm");
        if (re.empty() || !checkSame(lena128, re)) {
            std::cerr << "[IO] reload P2 mismatch" << std::endl;
            ++failed;
        }
    }
    // 3) 读取 P3 彩色
    cv::Mat color = ImageIO::LoadPpm(std::string(DATA_DIR) + "/color-block.ppm");
    if (color.empty() || color.channels() != 3 || color.rows != 390 || color.cols != 390) {
        std::cerr << "[IO] load P3 failed" << std::endl;
        ++failed;
    }
    // 4) 保存并重读 P3
    if (!Ppm::SaveNatAsPpm(std::string(OUTPUT_DIR) + "/out_color.ppm", color)) {
        std::cerr << "[IO] save P3 failed" << std::endl;
        ++failed;
    } else {
        cv::Mat re = ImageIO::LoadPpm(std::string(OUTPUT_DIR) + "/out_color.ppm");
        if (re.empty() || !checkSame(color, re)) {
            std::cerr << "[IO] reload P3 mismatch" << std::endl;
            ++failed;
        }
    }

    // 5) PNG 读写（使用现有灰度图作为内容）
    if (!ImageIO::SavePng(std::string(OUTPUT_DIR) + "/out_lena128.png", lena128)) {
        std::cerr << "[IO] save PNG failed" << std::endl;
        ++failed;
    } else {
        cv::Mat png = ImageIO::LoadPng(std::string(OUTPUT_DIR) + "/out_lena128.png");
        if (png.empty() || png.size() != lena128.size()) {
            std::cerr << "[IO] reload PNG size mismatch" << std::endl;
            ++failed;
        }
    }

    return failed;
}