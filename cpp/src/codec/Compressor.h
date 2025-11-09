#pragma once
#include <vector>
#include "../data_structure/Triplet.h"
        
class Compressor {
    /**
     * @brief 将图像压缩并保存为 .trip 文件。
     * * 流程：统计背景色 -> 转换为三元组 -> 写入文件头 -> 写入数据。
     * @param file_path 目标 .trip 文件路径。
     * @param img 输入图像。
     * @return true 成功，false 失败。
     */
    static bool Save(const std::string& file_path, const cv::Mat& img);

    /**
     * @brief 加载 .trip 文件并重建图像。
     * * 流程：读取文件头校验魔数 -> 创建背景画布 -> 覆盖三元组像素。
     * @param file_path 源 .trip 文件路径。
     * @return cv::Mat 重建的图像。
     */
    static cv::Mat Load(const std::string& file_path);
};