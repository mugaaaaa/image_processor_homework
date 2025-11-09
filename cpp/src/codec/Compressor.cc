#include "Compressor.h"
#include <fstream>

// 为简洁，采用文本 .trip 格式：
// 头部一行：TRIP width height channels count bgB bgG bgR
// 后续每行一个三元组：row col v0 [v1 v2]

static bool writeHeader(std::ostream& os, const CompressedHeader& hdr) {
    os << "TRIP " << hdr.width_ << ' ' << hdr.height_ << ' ' << hdr.channels_ << ' '
       << hdr.count_ << ' ' << static_cast<int>(hdr.bg_color_[0]) << ' '
       << static_cast<int>(hdr.bg_color_[1]) << ' ' << static_cast<int>(hdr.bg_color_[2]) << '\n';
    return !os.fail();
}

static bool readHeader(std::istream& is, CompressedHeader& hdr) {
    std::string magic; is >> magic;
    if (magic != "TRIP") return false;
    is >> hdr.width_ >> hdr.height_ >> hdr.channels_ >> hdr.count_;
    int b,g,r; is >> b >> g >> r;
    hdr.bg_color_[0] = static_cast<uint8_t>(b);
    hdr.bg_color_[1] = static_cast<uint8_t>(g);
    hdr.bg_color_[2] = static_cast<uint8_t>(r);
    return !is.fail();
}

bool Compressor::Save(const std::string& file_path, const cv::Mat& img) {
    if (img.empty()) return false;
    uint8_t bg[3] = {0,0,0};
    TripletUtils::findBackgroundColor(img, bg);
    std::vector<TripletNode> triplets;
    TripletUtils::matToTriplets(img, bg, triplets);
    CompressedHeader hdr{};
    hdr.width_ = img.cols; hdr.height_ = img.rows; hdr.channels_ = img.channels();
    hdr.count_ = triplets.size();
    hdr.bg_color_[0] = bg[0]; hdr.bg_color_[1] = bg[1]; hdr.bg_color_[2] = bg[2];
    std::ofstream ofs(file_path);
    if (!ofs) return false;
    if (!writeHeader(ofs, hdr)) return false;
    for (const auto& t : triplets) {
        ofs << t.row_ << ' ' << t.col_ << ' ' << static_cast<int>(t.val_[0]);
        if (hdr.channels_ == 3) {
            ofs << ' ' << static_cast<int>(t.val_[1]) << ' ' << static_cast<int>(t.val_[2]);
        }
        ofs << '\n';
    }
    return true;
}

cv::Mat Compressor::Load(const std::string& file_path) {
    std::ifstream ifs(file_path);
    if (!ifs) return cv::Mat();
    CompressedHeader hdr{};
    if (!readHeader(ifs, hdr)) return cv::Mat();
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
    cv::Mat img;
    TripletUtils::tripletsToMat(triplets, hdr.width_, hdr.height_, hdr.channels_, hdr.bg_color_, img);
    return img;
}