#pragma once
#include <cstdint>
#include <vector>
#include <opencv2/core/mat.hpp>

/**
 * @brief 三元组节点结构体，表示图像中非背景色的像素信息 
 */
struct TripletNode {
    int row_;
    int col_;
    uint8_t val_[3]; ///< 彩色图时 val_[0]: B, val_[1]: G, val_[2]: R(与 OpenCV 的神奇顺序对齐）， 灰度图时仅用 val_[0] 表示灰度
};


/**
 * @brief 压缩文件头结构体，存储图像的基本信息
 */
struct CompressedHeader {
    char magic_[4] = {'T', 'R', 'I', 'P'}; // 魔法数字，作为文件标识
    int32_t width_;
    int32_t height_;
    int32_t channels_; ///< 通道数，1表示灰度图，3表示彩色图
    uint64_t count_; ///< 三元组数量
    uint8_t bg_color_[3]; ///< 背景色，彩色图时存储BGR ，灰度图时仅用bg_color_[0]
}

/**
 * @brief Triplet 相关工具类，包含背景色统计、Mat 与三元组的转换等功能
 */
class TripletUtils {
public:
    /**
     * @brief 统计图像中出现频率最高的颜色作为背景色
     * 
     * @param img[in] 输入图像
     * @param bg_color[out] 接受背景色的数组，彩色图时为3个元素，灰度图时仅用第一个元素
     */
    static void findBackgroundColor(const cv::Mat& img, uint8_t bg_color[3]);

    /**
     * @brief 将 cv::Mat 图像转换为三元组表示
     * 
     * @param img[in] 输入图形
     * @param bg_color[in] 背景颜色
     * @param triplets[out] 接受三元组结果的向量
     */
    static void matToTriplets(const cv::Mat& img, const uint8_t bg_color[3], std::vector<TripletNode>& triplets);

    /**
     * @brief 将三元组表示转换回 cv::Mat 图像
     * 
     * @param triplets[in] 输入的三元组
     * @param width[in] 图像宽度
     * @param height[in] 图像高度
     * @param channels[in] 通道数量
     * @param bg_color[in] 背景颜色
     * @param img[out] 接受转换后图像的 cv::Mat 对象
     */
    static void tripletsToMat(const std::vector<TripletNode>& triplets, int width, int height, int channels, const uint8_t bg_color[3], cv::Mat& img);
}