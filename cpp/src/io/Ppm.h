#pragma once

#include <string>
#include <opencv2/opencv.hpp>

class Ppm {
public:
    /**
     * @brief 手动读取 PPM 文件(P2 或 P3 格式)。
     * 并在读取数据时将 RGB 转换为 OpenCV 默认的 BGR 顺序。
     */
    static cv::Mat loadPpmAsMat(const std::string& file_path);

    /**
     * @brief 手动保存为 PPM 格式。
     * 输入图像 (CV_8UC1 或 CV_8UC3, 分别用 P2 或 P3 格式保存)。
     */
    static bool saveNatAsPpm(const std::string& file_path, const cv::Mat& img);
};