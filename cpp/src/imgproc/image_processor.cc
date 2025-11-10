/**
 * @file ImageProcessor.cc
 * @author Runhui Mo (github.com/mugaaaaa)
 * @brief 
 * @version 0.1
 * @date 2025-11-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "image_processor.h"
#include <cmath>

// 辅助函数：将像素值 clamp 到 [0, 255] 范围
static inline uint8_t Clamp255(int v) { 
    return static_cast<uint8_t>(v < 0 ? 0 : (v > 255 ? 255 : v)); 
}

// 将彩色图像转换为灰度图像
cv::Mat Processor::ToGray(const cv::Mat& input) {
    // 输入为空或不是三通道图像时返回空 cv::Mat;
    if (input.empty()) return cv::Mat();
    if (input.channels() != 3) return cv::Mat();

    // 初始化空的灰度图像并逐个填充利用经验公式计算得到的灰度值
    cv::Mat gray(input.rows, input.cols, CV_8UC1);
    for (int r = 0; r < input.rows; ++r) {
        const cv::Vec3b* inrow = input.ptr<cv::Vec3b>(r);   // 获取 input 图像第 r 行的起始指针 
        uint8_t* outrow = gray.ptr<uint8_t>(r);     // 获取 gray 图像第 r 行的起始指针
        for (int c = 0; c < input.cols; ++c) {
            const cv::Vec3b& bgr = inrow[c];    // 通过行指针 inrow 和列偏移 c 访问到当前的 cv::Vec3b 像素
            // Gray = 0.299*R + 0.587*G + 0.114*B，按 BGR 顺序填充之后 clamp
            int g = static_cast<int>(0.299 * bgr[2] + 0.587 * bgr[1] + 0.114 * bgr[0]);
            outrow[c] = Clamp255(g);
        }
    }

    return gray;
}

// 图像缩放，用双线性插值实现
cv::Mat Processor::Resize(const cv::Mat& input, int new_width, int new_height) {
    // 有效性检查，输入为空或新宽度/高度非正时返回空 cv::Mat
    if (input.empty() || new_width <= 0 || new_height <= 0) return cv::Mat();
    int ch = input.channels();

    // 初始化空输出图像
    cv::Mat out(new_height, new_width, ch == 1 ? CV_8UC1 : CV_8UC3);

    // 计算输入图像到输出图像的缩放比例
    double scale_x = static_cast<double>(input.cols) / new_width;
    double scale_y = static_cast<double>(input.rows) / new_height;

    for (int y = 0; y < new_height; ++y) {
        // 计算输出图像中当前像素 (x, y) 的中心点在输入图像上对应的垂直浮点坐标 src_y
        // “y + 0.5” 将当前输出像素的整数行坐标 y 转换为其中心点的浮点行坐标
        // “- 0.5” 是为了将 src_y 映射到输入图像的中心位置
        double src_y = (y + 0.5) * scale_y - 0.5;

        // 找到 src_y 上方（或等于 src_y ）最近的整数行坐标 y0 和 src_y 下方最近的整数行坐标 y1
        int y0 = static_cast<int>(floor(src_y));
        int y1 = y0 + 1;

        // 计算 src_y 在 y0 和 y1 之间的相对位置，作为垂直方向的插值权重
        double wy = src_y - y0;
        
        // 处理 src_y 超出输入图像边界的情况
        if (y0 < 0) { y0 = 0; y1 = 0; wy = 0; }
        if (y1 >= input.rows) { y1 = input.rows - 1; y0 = y1; wy = 0; }

        for (int x = 0; x < new_width; ++x) {
            // 计算输出图像中当前像素 (x, y) 的中心点在输入图像上对应的水平浮点坐标 src_x
            double src_x = (x + 0.5) * scale_x - 0.5;

            // 找到 src_x 左侧（或等于 src_x ）最近的整数列坐标 x0 和 src_x 右侧最近的整数列坐标 x1
            int x0 = static_cast<int>(floor(src_x));
            int x1 = x0 + 1;

            // 计算 src_x 在 x0 和 x1 之间的相对位置，作为水平方向的插值权重
            double wx = src_x - x0;
            
            // 处理 src_x 超出输入图像边界的情况
            if (x0 < 0) { x0 = 0; x1 = 0; wx = 0; }
            if (x1 >= input.cols) { x1 = input.cols - 1; x0 = x1; wx = 0; }

            // 计算出双线性插值的结果并写入输出图像
            if (ch == 1) {  // 单通道时直接使用 uint8_t 类型
                const uint8_t Q00 = input.at<uint8_t>(y0, x0);
                const uint8_t Q10 = input.at<uint8_t>(y0, x1);
                const uint8_t Q01 = input.at<uint8_t>(y1, x0);
                const uint8_t Q11 = input.at<uint8_t>(y1, x1);

                // 计算插值结果并写入输出图像
                double v = (1 - wx) * (1 - wy) * Q00 + wx * (1 - wy) * Q10 + (1 - wx) * wy * Q01 + wx * wy * Q11;
                out.at<uint8_t>(y, x) = Clamp255(static_cast<int>(std::round(v)));
            } else {    // 三通道时使用 cv::Vec3b 类型
                const cv::Vec3b Q00 = input.at<cv::Vec3b>(y0, x0);
                const cv::Vec3b Q10 = input.at<cv::Vec3b>(y0, x1);
                const cv::Vec3b Q01 = input.at<cv::Vec3b>(y1, x0);
                const cv::Vec3b Q11 = input.at<cv::Vec3b>(y1, x1);

                // 计算出浮点插值结果并四舍五入转换回 uint8_t 类型
                cv::Vec3d v = (1 - wx) * (1 - wy) * cv::Vec3d(Q00) + wx * (1 - wy) * cv::Vec3d(Q10) + (1 - wx) * wy * cv::Vec3d(Q01) + wx * wy * cv::Vec3d(Q11);
                cv::Vec3b vb(static_cast<uint8_t>(std::round(v[0])), static_cast<uint8_t>(std::round(v[1])), static_cast<uint8_t>(std::round(v[2])));
                
                // 将计算并转换好的最终像素颜色值 vb 赋值给输出图像 out 中当前 (y, x) 位置的像素
                out.at<cv::Vec3b>(y, x) = vb;
            }
        }
    }

    return out;
}