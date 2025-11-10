/**
 * @file Compressor.cc
 * @author Runhui Mo (github.com/mugaaaaa)
 * @brief Compressor 类静态方法实现
 *
 * @details 三元组存储格式：
 * 文件头：TRIP width height channels count bgB bgG bgR\n
 * 后续每行一个三元组：row col v0 [v1 v2]\n
 *
 * @version 0.1
 * @date 2025-11-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "Compressor.h"
#include <fstream>

// 写入 .trip 文件头
static bool writeHeader(std::ostream& os, const CompressedHeader& hdr) {
    // 写入魔法数字，宽，高，通道数，三元组数量，背景色等信息
    os << "TRIP " << hdr.width_ << ' ' << hdr.height_ << ' ' << hdr.channels_ << ' '
       << hdr.count_ << ' ' << static_cast<int>(hdr.bg_color_[0]) << ' '
       << static_cast<int>(hdr.bg_color_[1]) << ' ' << static_cast<int>(hdr.bg_color_[2]) << '\n';
    
    return !os.fail();
}

// 读取 .trip 文件头
static bool readHeader(std::istream& is, CompressedHeader& hdr) {
    // 读取魔法数字，如果不是 ”TRIP“ 则返回 false
    std::string magic; is >> magic;
    if (magic != "TRIP") return false;

    // 读取文件头信息
    is >> hdr.width_ >> hdr.height_ >> hdr.channels_ >> hdr.count_;
    int b,g,r; is >> b >> g >> r;
    hdr.bg_color_[0] = static_cast<uint8_t>(b);
    hdr.bg_color_[1] = static_cast<uint8_t>(g);
    hdr.bg_color_[2] = static_cast<uint8_t>(r);
    
    return !is.fail();
}

// 将图像压缩并保存为 .trip 文件
bool Compressor::Save(const std::string& file_path, const cv::Mat& img) {
    if (img.empty()) return false;

    // 创建 bg 并接受 findBackgroundColor 的结果作为背景色
    uint8_t bg[3] = {0,0,0};
    TripletUtils::findBackgroundColor(img, bg);

    // 接收 matToTriplets 的结果作为三元组表示
    std::vector<TripletNode> triplets;
    TripletUtils::matToTriplets(img, bg, triplets);

    // 初始化并填充文件头部信息
    CompressedHeader hdr{};
    hdr.width_ = img.cols; hdr.height_ = img.rows; hdr.channels_ = img.channels();
    hdr.count_ = triplets.size();
    hdr.bg_color_[0] = bg[0]; hdr.bg_color_[1] = bg[1]; hdr.bg_color_[2] = bg[2];
    
    // 打开文件流
    std::ofstream ofs(file_path);
    if (!ofs) return false;

    // 写入文件头
    if (!writeHeader(ofs, hdr)) return false;

    // 写入三元组，后续可能可以继续优化
    for (const auto& t : triplets) {
        ofs << t.row_ << ' ' << t.col_ << ' ' << static_cast<int>(t.val_[0]);
        if (hdr.channels_ == 3) {
            ofs << ' ' << static_cast<int>(t.val_[1]) << ' ' << static_cast<int>(t.val_[2]);
        }
        ofs << '\n';
    }
    
    return true;
}

// 加载 .trip 文件并重建图像
cv::Mat Compressor::Load(const std::string& file_path) {
    // 打开文件流，如果打开失败就返回空 cv::Mat
    std::ifstream ifs(file_path);
    if (!ifs) return cv::Mat();

    // 读取文件头信息，如果读取失败就返回空 cv::Mat
    CompressedHeader hdr{};
    if (!readHeader(ifs, hdr)) return cv::Mat();

    // 预分配空间，避免后续 resize 导致的性能问题
    std::vector<TripletNode> triplets;
    triplets.reserve(static_cast<size_t>(hdr.count_));

    // 读入三元组为 std::vector<TripletNode>
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

    // 调用三元组工具类的 tripletsToMat 方法吧上一步的 std::vector<TripletNode> 转为 cv::Mat
    cv::Mat img;
    TripletUtils::tripletsToMat(triplets, hdr.width_, hdr.height_, hdr.channels_, hdr.bg_color_, img);
    
    return img;
}