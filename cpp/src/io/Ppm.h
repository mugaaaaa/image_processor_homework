#pragma once

#include <string>
#include <opencv2/opencv.hpp>

class Ppm {
    /**
     * @brief 手动读取 PPM 文件(P2 或 P3 格式)。
     * * 并在读取数据时将 RGB 转换为 OpenCV 默认的 BGR 顺序。
     * @param file_path PPM 文件路径。
     * @return cv::Mat 读取的图像 (CV_8UC1 或 CV_8UC3)。
     */
    static cv::Mat loadPpmAsMat(const std::string& file_path);

    /**
     * @brief 手动保存为 PPM 格式。
     * @param file_path 目标路径。
     * @param img 输入图像 (CV_8UC1 或 CV_8UC3, 分别用 P2 或 P3 格式保存)。
     * @return true 保存成功
     * @return false 保存失败
     */
    static bool saveNatAsPpm(const std::string& file_path, const cv::Mat& img);
};