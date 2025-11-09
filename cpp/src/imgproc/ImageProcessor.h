#pragma once

#include <opencv2/opencv.hpp>

/**
 * @brief 图像处理算法集合类。
 * * 包含手动实现的经典图像处理算法。
 */
class Processor {
 public:
  /**
   * @brief 将彩色图像转换为灰度图像。
   * * 经验公式：Gray = 0.299*R + 0.587*G + 0.114*B
   * @param input 输入图像 (CV_8UC3)。
   * @return cv::Mat 灰度图像 (CV_8UC1)。
   */
  static cv::Mat ToGray(const cv::Mat& input);

  /**
   * @brief 使用双线性插值调整图像尺寸。
   * * 支持单通道(灰度)和三通道(彩色)图像。
   * @param input 输入图像。
   * @param new_width 目标宽度。
   * @param new_height 目标高度。
   * @return cv::Mat 调整尺寸后的图像。
   */
  static cv::Mat Resize(const cv::Mat& input, int new_width,
                                int new_height);
};