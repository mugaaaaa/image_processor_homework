/**
 * @file Ppm.cc
 * @author Runhui Mo (github.com/mugaaaaa)
 * @brief ppm 读写类实现
 * @version 0.1
 * @date 2025-11-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "Ppm.h"
#include <fstream>
#include <sstream>
#include <string>

// 辅助函数：读取下一个有效的 token，跳过注释和空白
static bool readToken(std::istream& is, std::string& tok) {
    tok.clear();
    char ch;
    
    // 跳过空白字符和注释行（可能）
    while (is.get(ch)) {
        if (ch == '#') {
            std::string dummy;
            std::getline(is, dummy);
            continue;
        }
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            tok.push_back(ch);
            break;
        }
    }

    // 继续读取直到下一个空白字符
    while (is.get(ch)) {
        if (std::isspace(static_cast<unsigned char>(ch))) break;
        tok.push_back(ch);
    }

    return !tok.empty();
}

// 读取 PPM 文件(P2 或 P3 格式) 为 cv::Mat
cv::Mat Ppm::LoadPpmAsMat(const std::string& file_path) {
    // 打开文件流，若打开失败则返回空 cv::Mat
    std::ifstream ifs(file_path);
    if (!ifs) return cv::Mat();

    // 读取魔术数字，如果读入异常或魔术数字不合法则返回空 cv::Mat
    std::string magic;
    if (!readToken(ifs, magic)) return cv::Mat();
    if (magic != "P2" && magic != "P3") return cv::Mat();

    // 读取宽度、高度、最大像素值，读入异常或不合法则返回空 cv::Mat
    std::string wtok, htok, maxtok;
    if (!readToken(ifs, wtok)) return cv::Mat();
    if (!readToken(ifs, htok)) return cv::Mat();
    if (!readToken(ifs, maxtok)) return cv::Mat();
    int width = std::stoi(wtok);
    int height = std::stoi(htok);
    int maxv = std::stoi(maxtok);
    if (width <= 0 || height <= 0 || maxv <= 0) return cv::Mat();

    if (magic == "P2") {
        cv::Mat img(height, width, CV_8UC1);
        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
                // 读取下一个像素值 token
                std::string vtok;
                if (!readToken(ifs, vtok)) return cv::Mat();

                // 将像素值从字符串转换为整数，并进行范围检查
                int v = std::stoi(vtok);
                if (v < 0) v = 0; if (v > maxv) v = maxv;

                // 归一化到 0-255 范围并存储到图像中
                uint8_t u = static_cast<uint8_t>(v * 255 / maxv);
                img.at<uint8_t>(r, c) = u;
            }
        }

        return img;
    } else { // P3
        cv::Mat img(height, width, CV_8UC3);
        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
                // 读取 R、G、B 三个通道的像素值 token
                std::string rtok, gtok, btok;
                if (!readToken(ifs, rtok)) return cv::Mat();
                if (!readToken(ifs, gtok)) return cv::Mat();
                if (!readToken(ifs, btok)) return cv::Mat();

                // 将像素值从字符串转换为整数，并进行范围检查
                int R = std::stoi(rtok);
                int G = std::stoi(gtok);
                int B = std::stoi(btok);
                if (R < 0) R = 0; if (R > maxv) R = maxv;
                if (G < 0) G = 0; if (G > maxv) G = maxv;
                if (B < 0) B = 0; if (B > maxv) B = maxv;

                // BGR 顺序存储到图像中
                img.at<cv::Vec3b>(r, c) = cv::Vec3b(
                    static_cast<uint8_t>(B * 255 / maxv),
                    static_cast<uint8_t>(G * 255 / maxv),
                    static_cast<uint8_t>(R * 255 / maxv));
            }
        }
        return img;
    }
}

// 保存 cv::Mat 为 PPM 格式
bool Ppm::SaveNatAsPpm(const std::string& file_path, const cv::Mat& img) {
    // 打开文件流，若打开失败则返回 false
    std::ofstream ofs(file_path);
    if (!ofs) return false;

    // 准备 PPM 头信息：宽度、高度、最大像素值
    int width = img.cols, height = img.rows;
    int channels = img.channels();
    int maxv = 255;

    if (channels == 1) {  // P2
        // 写入头信息
        ofs << "P2\n" << width << " " << height << "\n" << maxv << "\n";

        // 写入数据 
        for (int r = 0; r < height; ++r) {
            const uint8_t* rowp = img.ptr<uint8_t>(r);
            for (int c = 0; c < width; ++c) {
                ofs << static_cast<int>(rowp[c]);
                if (c + 1 < width) ofs << ' ';
            }
            ofs << '\n';
        }

        return true;
    } else if (channels == 3) { // P3
        // 写入头信息
        ofs << "P3\n" << width << " " << height << "\n" << maxv << "\n";

        // 写入数据 
        for (int r = 0; r < height; ++r) {
            const cv::Vec3b* rowp = img.ptr<cv::Vec3b>(r);

            for (int c = 0; c < width; ++c) {
                const cv::Vec3b& bgr = rowp[c];
                // 用 RGB 顺序写入像素值
                ofs << static_cast<int>(bgr[2]) << ' '
                    << static_cast<int>(bgr[1]) << ' '
                    << static_cast<int>(bgr[0]);
                if (c + 1 < width) ofs << ' ';
            }

            ofs << '\n';
        }

        return true;
    }

    return false;
}