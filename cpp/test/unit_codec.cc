// 压缩/解压模块单元测试：Triplet 文本格式 Save/Load 循环一致
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../src/codec/compressor.h"
#include "../src/io/image_io.h"
#include <cstring>

static bool compareMat(const cv::Mat& a, const cv::Mat& b) {
    if (a.size() != b.size() || a.type() != b.type()) return false;
    return std::memcmp(a.data, b.data, a.total() * a.elemSize()) == 0;
}

int test_codec() {
    int failed = 0;
    // 使用彩色块图测试，便于出现非均匀背景
    cv::Mat color = ImageIO::LoadPpm(std::string(DATA_DIR) + "/color-block.ppm");
    if (color.empty()) { std::cerr << "[Codec] load color failed" << std::endl; return ++failed; }

    const std::string trip_path = std::string(OUTPUT_DIR) + "/out_color.trip";
    if (!Compressor::Save(trip_path, color)) {
        std::cerr << "[Codec] Save trip failed" << std::endl; ++failed;
    } else {
        cv::Mat recon = Compressor::Load(trip_path);
        if (recon.empty()) {
            std::cerr << "[Codec] Load trip empty" << std::endl; ++failed;
        } else if (!compareMat(color, recon)) {
            std::cerr << "[Codec] Round-trip mismatch" << std::endl; ++failed;
        }
    }
    
    // 灰度图也测试一次
    cv::Mat gray = ImageIO::LoadPpm(std::string(DATA_DIR) + "/lena-128-gray.ppm");
    if (gray.empty()) { std::cerr << "[Codec] load gray failed" << std::endl; return ++failed; }
    const std::string trip_path2 = std::string(OUTPUT_DIR) + "/out_lena128.trip";
    if (!Compressor::Save(trip_path2, gray)) { std::cerr << "[Codec] Save gray trip failed" << std::endl; ++failed; }
    else {
        cv::Mat recon2 = Compressor::Load(trip_path2);
        if (recon2.empty() || !compareMat(gray, recon2)) { std::cerr << "[Codec] Round-trip gray mismatch" << std::endl; ++failed; }
    }

    return failed;
}