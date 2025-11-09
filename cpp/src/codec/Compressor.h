#pragma once
#include <string>
#include <vector>
#include <opencv2/core/mat.hpp>
#include "../data_structure/Triplet.h"

class Compressor {
public:
    /**
     * @brief 将图像压缩并保存为 .trip 文件。
     * 流程：统计背景色 -> 转换为三元组 -> 写入文件头 -> 写入数据。
     */
    static bool Save(const std::string& file_path, const cv::Mat& img);

    /**
     * @brief 加载 .trip 文件并重建图像。
     * 流程：读取文件头校验魔数 -> 创建背景画布 -> 覆盖三元组像素。
     */
    static cv::Mat Load(const std::string& file_path);
};