/**
 * @file Ppm.h
 * @author Runhui Mo (github.com/mugaaaaa)
 * @brief ppm 读写类声明，主要是 pmm 和 cv::Mat 之间的转换
 * @version 0.1
 * @date 2025-11-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <string>
#include <opencv2/opencv.hpp>

class Ppm {
public:
    /**
     * @brief 读取 PPM 文件(P2 或 P3 格式) 为 cv::Mat
     * - 在读取数据时将 RGB 转换为 OpenCV 默认的 BGR 顺序
     */
    static cv::Mat LoadPpmAsMat(const std::string& file_path);

    /**
     * @brief 手动保存为 PPM 格式
     * - 输入 cv::Mat 格式图像 (CV_8UC1 或 CV_8UC3, 分别用 P2 或 P3 格式保存)
     */
    static bool SaveNatAsPpm(const std::string& file_path, const cv::Mat& img);
};