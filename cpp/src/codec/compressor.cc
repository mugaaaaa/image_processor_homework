/**
 * @file compressor.cc
 * @author Runhui Mo (github.com/mugaaaaa)
 * @brief Compressor 类静态方法实现
 *
 * @details 三元组存储格式：
 * 文件头（文本）：TRIP width height channels count bgB bgG bgR\n
 * 数据段（二进制）：按顺序写入 count 个节点，每个节点包含
 *   int32 row, int32 col, uint8 v0[, uint8 v1, uint8 v2]
 *
 * @version 0.1
 * @date 2025-11-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "compressor.h"
#include <fstream>
#include <cstdint>
#include <limits>


// 写入 .trip 文件头
static bool WriteHeader(std::ostream& os, const CompressedHeader& hdr) {
    // 写入魔术数字，宽，高，通道数，三元组数量，背景色等信息
    os << "TRIP " << hdr.width_ << ' ' << hdr.height_ << ' ' << hdr.channels_ << ' '
       << static_cast<unsigned long long>(hdr.count_) << ' ' << static_cast<int>(hdr.bg_color_[0]) << ' '
       << static_cast<int>(hdr.bg_color_[1]) << ' ' << static_cast<int>(hdr.bg_color_[2]) << '\n';
    
    return !os.fail();
}

// 读取 .trip 文件头
static bool ReadHeader(std::istream& is, CompressedHeader& hdr) {
    // 读取魔法数字，如果不是 ”TRIP“ 则返回 false
    std::string magic; is >> magic;
    if (magic != "TRIP") return false;

    // 读取文件头信息
    is >> hdr.width_ >> hdr.height_ >> hdr.channels_ >> hdr.count_;
    int b,g,r; is >> b >> g >> r;
    hdr.bg_color_[0] = static_cast<uint8_t>(b);
    hdr.bg_color_[1] = static_cast<uint8_t>(g);
    hdr.bg_color_[2] = static_cast<uint8_t>(r);
    
    // 丢弃到行尾，确保后续二进制读取从正确位置开始
    is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return !is.fail();
}

// 将图像压缩并保存为 .trip 文件
bool Compressor::Save(const std::string& file_path, const cv::Mat& img) {
    if (img.empty()) return false;

    // 创建 bg 并接受 FindBackgroundColor 的结果作为背景色
    uint8_t bg[3] = {0,0,0};
    TripletUtils::FindBackgroundColor(img, bg);

    // 接收 MatToTriplets 的结果作为三元组表示
    std::vector<TripletNode> triplets;
    TripletUtils::MatToTriplets(img, bg, triplets);

    // 初始化并填充文件头部信息
    CompressedHeader hdr{};
    hdr.width_ = img.cols; hdr.height_ = img.rows; hdr.channels_ = img.channels();
    hdr.count_ = triplets.size();
    hdr.bg_color_[0] = bg[0]; hdr.bg_color_[1] = bg[1]; hdr.bg_color_[2] = bg[2];
    
    // 打开文件流（二进制），头部以文本写入，数据段以二进制写入
    std::ofstream ofs(file_path, std::ios::binary);
    if (!ofs) return false;

    // 写入文件头
    if (!WriteHeader(ofs, hdr)) return false;

    // 写入三元组数据段（二进制）：int32 row, int32 col, uint8 v0[, v1, v2]
    for (const auto& t : triplets) {
        int32_t row = static_cast<int32_t>(t.row_);
        int32_t col = static_cast<int32_t>(t.col_);
        uint8_t v0 = t.val_[0];
        ofs.write(reinterpret_cast<const char*>(&row), sizeof(row));
        ofs.write(reinterpret_cast<const char*>(&col), sizeof(col));
        ofs.write(reinterpret_cast<const char*>(&v0), sizeof(v0));
        if (hdr.channels_ == 3) {
            uint8_t v1 = t.val_[1];
            uint8_t v2 = t.val_[2];
            ofs.write(reinterpret_cast<const char*>(&v1), sizeof(v1));
            ofs.write(reinterpret_cast<const char*>(&v2), sizeof(v2));
        }
        if (!ofs) return false;
    }
    
    return true;
}

// 加载 .trip 文件并重建图像
cv::Mat Compressor::Load(const std::string& file_path) {
    // 打开文件流（二进制），如果打开失败就返回空 cv::Mat
    std::ifstream ifs(file_path, std::ios::binary);
    if (!ifs) return cv::Mat();

    // 读取文件头信息，如果读取失败就返回空 cv::Mat
    CompressedHeader hdr{};
    if (!ReadHeader(ifs, hdr)) return cv::Mat();

    // 预分配空间，避免后续 resize 导致的性能问题
    std::vector<TripletNode> triplets;
    triplets.reserve(static_cast<size_t>(hdr.count_));

    // 读入三元组为 std::vector<TripletNode>（二进制读取）
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

    // 调用三元组工具类的 TripletsToMat 方法吧上一步的 std::vector<TripletNode> 转为 cv::Mat
    cv::Mat img;
    TripletUtils::TripletsToMat(triplets, hdr.width_, hdr.height_, hdr.channels_, hdr.bg_color_, img);
    
    return img;
}