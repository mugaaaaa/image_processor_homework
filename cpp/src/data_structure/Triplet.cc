#include "Triplet.h"
#include <unordered_map>
#include <array>

void TripletUtils::findBackgroundColor(const cv::Mat& img, uint8_t bg_color[3]) {
    int channels = img.channels();
    if (channels == 1) {
        std::array<size_t, 256> hist{};
        for (int r = 0; r < img.rows; ++r) {
            const uint8_t* rowp = img.ptr<uint8_t>(r);
            for (int c = 0; c < img.cols; ++c) {
                ++hist[rowp[c]];
            }
        }
        size_t maxcnt = 0; int maxval = 0;
        for (int v = 0; v < 256; ++v) {
            if (hist[v] > maxcnt) { maxcnt = hist[v]; maxval = v; }
        }
        bg_color[0] = static_cast<uint8_t>(maxval);
        bg_color[1] = 0; bg_color[2] = 0;
    } else if (channels == 3) {
        std::unordered_map<uint32_t, size_t> hist;
        hist.reserve(static_cast<size_t>(img.rows) * img.cols / 8 + 256);
        for (int r = 0; r < img.rows; ++r) {
            const cv::Vec3b* rowp = img.ptr<cv::Vec3b>(r);
            for (int c = 0; c < img.cols; ++c) {
                const cv::Vec3b& pix = rowp[c]; // B,G,R
                uint32_t key = (static_cast<uint32_t>(pix[0]) << 16) |
                               (static_cast<uint32_t>(pix[1]) << 8) |
                               (static_cast<uint32_t>(pix[2]));
                ++hist[key];
            }
        }
        uint32_t best_key = 0; size_t best_cnt = 0;
        for (const auto& kv : hist) {
            if (kv.second > best_cnt) { best_cnt = kv.second; best_key = kv.first; }
        }
        bg_color[0] = static_cast<uint8_t>((best_key >> 16) & 0xFF);
        bg_color[1] = static_cast<uint8_t>((best_key >> 8) & 0xFF);
        bg_color[2] = static_cast<uint8_t>(best_key & 0xFF);
    } else {
        // 非 1/3 通道，默认背景设为 0
        bg_color[0] = bg_color[1] = bg_color[2] = 0;
    }
}

void TripletUtils::matToTriplets(const cv::Mat& img, const uint8_t bg_color[3], std::vector<TripletNode>& triplets) {
    triplets.clear();
    int channels = img.channels();
    if (channels == 1) {
        for (int r = 0; r < img.rows; ++r) {
            const uint8_t* rowp = img.ptr<uint8_t>(r);
            for (int c = 0; c < img.cols; ++c) {
                uint8_t v = rowp[c];
                if (v == bg_color[0]) continue;
                TripletNode node; node.row_ = r; node.col_ = c;
                node.val_[0] = v; node.val_[1] = 0; node.val_[2] = 0;
                triplets.push_back(node);
            }
        }
    } else if (channels == 3) {
        for (int r = 0; r < img.rows; ++r) {
            const cv::Vec3b* rowp = img.ptr<cv::Vec3b>(r);
            for (int c = 0; c < img.cols; ++c) {
                const cv::Vec3b& pix = rowp[c];
                if (pix[0] == bg_color[0] && pix[1] == bg_color[1] && pix[2] == bg_color[2]) continue;
                TripletNode node; node.row_ = r; node.col_ = c;
                node.val_[0] = pix[0]; node.val_[1] = pix[1]; node.val_[2] = pix[2];
                triplets.push_back(node);
            }
        }
    }
}

void TripletUtils::tripletsToMat(const std::vector<TripletNode>& triplets, int width, int height, int channels, const uint8_t bg_color[3], cv::Mat& img) {
    if (channels == 1) {
        img = cv::Mat(height, width, CV_8UC1, cv::Scalar(bg_color[0]));
        for (const auto& node : triplets) {
            if (node.row_ >= 0 && node.row_ < height && node.col_ >= 0 && node.col_ < width) {
                img.at<uint8_t>(node.row_, node.col_) = node.val_[0];
            }
        }
    } else if (channels == 3) {
        img = cv::Mat(height, width, CV_8UC3, cv::Scalar(bg_color[0], bg_color[1], bg_color[2]));
        for (const auto& node : triplets) {
            if (node.row_ >= 0 && node.row_ < height && node.col_ >= 0 && node.col_ < width) {
                img.at<cv::Vec3b>(node.row_, node.col_) = cv::Vec3b(node.val_[0], node.val_[1], node.val_[2]);
            }
        }
    } else {
        // 不支持的通道，创建空图
        img = cv::Mat();
    }
}