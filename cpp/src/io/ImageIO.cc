/**
 * @file ImageIO.cc
 * @author Runhui Mo (github.com/mugaaaaa)
 * @brief ImageIO 类静态方法实现
 * @version 0.1
 * @date 2025-11-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "ImageIO.h"
#include <opencv2/imgcodecs.hpp>
#include "Ppm.h"
#include <fstream>

cv::Mat ImageIO::loadPng(const std::string& file_path) {
    return cv::imread(file_path, cv::IMREAD_UNCHANGED);
}

cv::Mat ImageIO::loadPpm(const std::string& file_path) {
    return Ppm::loadPpmAsMat(file_path);
}

// 读取文件头：TRIP width height channels count bgB bgG bgR
static bool parseHeader(std::istream& is, CompressedHeader& hdr) {
    std::string magic; is >> magic;
    if (magic != "TRIP") return false;
    is >> hdr.width_ >> hdr.height_ >> hdr.channels_ >> hdr.count_;
    int b, g, r; is >> b >> g >> r;
    hdr.bg_color_[0] = static_cast<uint8_t>(b);
    hdr.bg_color_[1] = static_cast<uint8_t>(g);
    hdr.bg_color_[2] = static_cast<uint8_t>(r);
    return !is.fail();
}

std::vector<TripletNode> ImageIO::loadTrip(const std::string& file_path) {
    std::ifstream ifs(file_path);
    if (!ifs) return {};
    CompressedHeader hdr{};
    if (!parseHeader(ifs, hdr)) return {};
    std::vector<TripletNode> triplets;
    triplets.reserve(static_cast<size_t>(hdr.count_));
    int row, col, v0, v1, v2;
    for (size_t i = 0; i < hdr.count_ && (ifs >> row >> col >> v0); ++i) {
        TripletNode node{}; node.row_ = row; node.col_ = col; node.val_[0] = static_cast<uint8_t>(v0);
        if (hdr.channels_ == 3) {
            if (!(ifs >> v1 >> v2)) break;
            node.val_[1] = static_cast<uint8_t>(v1);
            node.val_[2] = static_cast<uint8_t>(v2);
        } else {
            node.val_[1] = node.val_[2] = 0;
        }
        triplets.push_back(node);
    }
    return triplets;
}

bool ImageIO::savePng(const std::string& file_path, const cv::Mat& img) {
    return cv::imwrite(file_path, img);
}

bool ImageIO::savePpm(const std::string& file_path, const cv::Mat& img) {
    return Ppm::saveNatAsPpm(file_path, img);
}

bool ImageIO::saveTrip(const std::string& file_path,
                       int width,
                       int height,
                       int channels,
                       const uint8_t bg_color[3],
                       const std::vector<TripletNode>& triplets) {
    if (channels != 1 && channels != 3) return false;
    if (width <= 0 || height <= 0) return false;
    std::ofstream ofs(file_path);
    if (!ofs) return false;
    ofs << "TRIP " << width << ' ' << height << ' ' << channels << ' '
        << static_cast<unsigned long long>(triplets.size()) << ' '
        << static_cast<int>(bg_color[0]) << ' '
        << static_cast<int>(bg_color[1]) << ' '
        << static_cast<int>(bg_color[2]) << '\n';
    for (const auto& t : triplets) {
        ofs << t.row_ << ' ' << t.col_ << ' ' << static_cast<int>(t.val_[0]);
        if (channels == 3) {
            ofs << ' ' << static_cast<int>(t.val_[1]) << ' ' << static_cast<int>(t.val_[2]);
        }
        ofs << '\n';
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