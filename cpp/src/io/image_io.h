/**
 * @file image_io.h
 * @author Runhui Mo (github.com/mugaaaaa)
 * @brief 声明图像 I/O 接口类 ImageIO
 * @version 0.1
 * @date 2025-11-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <opencv2/core/mat.hpp>
#include "../data_structure/triplet.h"

/**
 * @brief 图像 I/O 接口类
 * 
 * @details 提供了加载和保存 .png、.ppm 和 .trip（自定义的压缩类型）图像的静态方法
 * - 和与 Node.js 互操作的接口。
 */
class ImageIO {
public:
    /**
     * @brief 从文件加载图像
     * 
     * @param file_path .png 文件路径
     * @return cv::Mat 加载的图像
     */
    static cv::Mat LoadPng(const std::string& file_path);

    /**
     * @brief 从文件加载三元组压缩图像
     * - 具体实现在 Ppm 部分，这里仅调用 Ppm::loadPpmAsMat
     * 
     * @param file_path .ppm 文件路径
     * @return cv::Mat 加载的图像
     */
    static cv::Mat LoadPpm(const std::string& file_path);

    /**
     * @brief 从文件加载三元组压缩图像
     * 
     * @param file_path .trip 文件路径
     * @return std::vector<TripletNode> 加载的三元组
     */
    static std::vector<TripletNode> LoadTrip(const std::string& file_path);

    /**
     * @brief 将图像保存到文件
     * 
     * @param file_path .png 文件路径
     * @param img 要保存的图像
     * @return true 保存成功
     * @return false 保存失败
     */
    static bool SavePng(const std::string& file_path, const cv::Mat& img);

    /**
     * @brief 将图像保存到文件
     * 具体实现在 Ppm 部分，这里仅调用 Ppm::saveNatAsPpm
     * 
     * @param file_path .ppm 文件路径
     * @param img 要保存的图像
     * @return true 保存成功
     * @return false 保存失败
     */
    static bool SavePpm(const std::string& file_path, const cv::Mat& img);

    /**
     * @brief 将图像保存到文件
     * 
     * @param file_path .trip 文件路径
     * @param triplets 要保存的三元组
     * @return true 保存成功
     * @return false 保存失败
     */
    static bool SaveTrip(const std::string& file_path, const std::vector<TripletNode>& triplets);

    /**
     * @brief 写入完整头部信息的.trip 保存接口。
     * 
     * @details 头部（文本）格式：TRIP width height channels count bgB bgG bgR\n
     * - channels 仅支持 1（灰度）或 3（彩色）；
     * - bg_color 为 BGR 顺序（灰度仅用 bg_color[0]）。
     * - 随后为二进制数据段，按节点顺序写入，共 count 个节点：
     *   每个节点依次写入 int32 row, int32 col, uint8 v0[, uint8 v1, uint8 v2]
     * 
     * @param file_path 输出 .trip 文件路径
     * @param width 图像宽度
     * @param height 图像高度
     * @param channels 通道数（1 或 3）
     * @param bg_color 背景色（BGR，灰度仅用第一个分量）
     * @param triplets 要写入的三元组数据
     * @return true 成功保存
     * @return false 失败（参数非法或文件不可写）
     */
    static bool SaveTrip(const std::string& file_path,
                         int width,
                         int height,
                         int channels,
                         const uint8_t bg_color[3],
                         const std::vector<TripletNode>& triplets);


    // =========================================================
    // Node.js 互操作接口 (Buffer I/O)
    // =========================================================

    /**
    * @brief 从内存缓冲区解码图像 (供 Node.js 调用)。
    * @param data 指向 buffer 数据的指针。
    * @param size buffer 数据长度。
    * @return cv::Mat 解码后的图像。
    */
    static cv::Mat DecodeFromBuffer(const uint8_t* data, size_t size);

    /**
    * @brief 将图像编码到内存缓冲区 (供 Node.js 调用)。
    * @param img 输入图像。
    * @param format 编码格式
    * @param[out] buffer 输出的二进制数据。
    * @return true 成功，false 失败。
    */
    static bool EncodeToBuffer(const cv::Mat& img, const std::string& format,
                                std::vector<uint8_t>* buffer);
};