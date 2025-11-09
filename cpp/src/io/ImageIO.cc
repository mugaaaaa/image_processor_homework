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

// 简易 .trip 读取：按文本格式读取，利于测试（header + triplets 行）
// 头：TRIP width height channels count bgB bgG bgR
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

bool ImageIO::saveTrip(const std::string& file_path, const std::vector<TripletNode>& triplets) {
    // 为简化，将 header 写为占位（无法得知宽高通道，仅用于单元测试配合 Compressor）
    // 单元测试中更常用 Compressor::Save/Load；这里仅提供最简写接口
    std::ofstream ofs(file_path);
    if (!ofs) return false;
    ofs << "TRIP 0 0 3 " << triplets.size() << " 0 0 0\n";
    for (const auto& t : triplets) {
        ofs << t.row_ << ' ' << t.col_ << ' '
            << static_cast<int>(t.val_[0]) << ' '
            << static_cast<int>(t.val_[1]) << ' '
            << static_cast<int>(t.val_[2]) << '\n';
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