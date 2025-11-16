/**
 * @file image_io.cc
 * @author Runhui Mo (github.com/mugaaaaa)
 * @brief ImageIO 类静态方法实现
 * @version 0.1
 * @date 2025-11-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "image_io.h"
#include <opencv2/imgcodecs.hpp>
#include "ppm.h"
#include <fstream>
#include <cstdint>
#include <limits>

cv::Mat ImageIO::LoadPng(const std::string& file_path) {
    return cv::imread(file_path, cv::IMREAD_UNCHANGED);
}

cv::Mat ImageIO::LoadPpm(const std::string& file_path) {
    return Ppm::LoadPpmAsMat(file_path);
}

// 读取文件头：TRIP width height channels count bgB bgG bgR
static bool ParseHeader(std::istream& is, CompressedHeader& hdr) {
    std::string magic; is >> magic;
    if (magic != "TRIP") return false;
    is >> hdr.width_ >> hdr.height_ >> hdr.channels_ >> hdr.count_;
    int b, g, r; is >> b >> g >> r;
    hdr.bg_color_[0] = static_cast<uint8_t>(b);
    hdr.bg_color_[1] = static_cast<uint8_t>(g);
    hdr.bg_color_[2] = static_cast<uint8_t>(r);
    // 丢弃到行尾，确保后续二进制读取从正确位置开始
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return !is.fail();
}

std::vector<TripletNode> ImageIO::LoadTrip(const std::string& file_path) {
    // 打开文件流，二进制模式
    std::ifstream ifs(file_path, std::ios::binary);
    if (!ifs) return {};

    // 初始化文件头结构体
    CompressedHeader hdr{};
    if (!ParseHeader(ifs, hdr)) return {};

    // 初始化三元组，预留空间
    std::vector<TripletNode> triplets;
    triplets.reserve(static_cast<size_t>(hdr.count_));


    for (uint64_t i = 0; i < hdr.count_; ++i) {
        int32_t row = 0, col = 0; uint8_t v0 = 0, v1 = 0, v2 = 0;
        ifs.read(reinterpret_cast<char*>(&row), sizeof(row));
        ifs.read(reinterpret_cast<char*>(&col), sizeof(col));
        ifs.read(reinterpret_cast<char*>(&v0), sizeof(v0));
        if (!ifs) break;
        TripletNode node{}; node.row_ = row; node.col_ = col; node.val_[0] = v0;
        if (hdr.channels_ == 3) {
            ifs.read(reinterpret_cast<char*>(&v1), sizeof(v1));
            ifs.read(reinterpret_cast<char*>(&v2), sizeof(v2));
            if (!ifs) break;
            node.val_[1] = v1; node.val_[2] = v2;
        } else {
            node.val_[1] = node.val_[2] = 0;
        }
        triplets.push_back(node);
    }
    return triplets;
}

bool ImageIO::SavePng(const std::string& file_path, const cv::Mat& img) {
    return cv::imwrite(file_path, img);
}

bool ImageIO::SavePpm(const std::string& file_path, const cv::Mat& img) {
    return Ppm::SaveNatAsPpm(file_path, img);
}

bool ImageIO::SaveTrip(const std::string& file_path, int width, int height, int channels, const uint8_t bg_color[3], const std::vector<TripletNode>& triplets) {
    // 校验参数
    if (channels != 1 && channels != 3) return false;
    if (width <= 0 || height <= 0) return false;

    // 打开文件流，二进制模式
    std::ofstream ofs(file_path, std::ios::binary);
    if (!ofs) return false;
    
    // 写入文件头
    ofs << "TRIP " << width << ' ' << height << ' ' << channels << ' '
        << static_cast<unsigned long long>(triplets.size()) << ' '
        << static_cast<int>(bg_color[0]) << ' '
        << static_cast<int>(bg_color[1]) << ' '
        << static_cast<int>(bg_color[2]) << '\n';

    // 二进制写入 triplets
    for (const auto& t : triplets) {
        int32_t row = static_cast<int32_t>(t.row_);
        int32_t col = static_cast<int32_t>(t.col_);
        uint8_t v0 = t.val_[0];
        ofs.write(reinterpret_cast<const char*>(&row), sizeof(row));
        ofs.write(reinterpret_cast<const char*>(&col), sizeof(col));
        ofs.write(reinterpret_cast<const char*>(&v0), sizeof(v0));
        if (channels == 3) {
            uint8_t v1 = t.val_[1];
            uint8_t v2 = t.val_[2];
            ofs.write(reinterpret_cast<const char*>(&v1), sizeof(v1));
            ofs.write(reinterpret_cast<const char*>(&v2), sizeof(v2));
        }
        if (!ofs) return false;
    }

    return true;
}

cv::Mat ImageIO::DecodeFromBuffer(const uint8_t* data, size_t size) {
    std::vector<uint8_t> buf(data, data + size);
    cv::Mat img = cv::imdecode(buf, cv::IMREAD_UNCHANGED);
    return img;
}

bool ImageIO::EncodeToBuffer(const cv::Mat& img, const std::string& format, std::vector<uint8_t>* buffer) {
    std::vector<int> params; // 默认参数
    return cv::imencode("." + format, img, *buffer, params);
}