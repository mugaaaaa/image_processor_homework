#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <opencv2/core/mat.hpp>

/**
 * @brief 图像 I/O 接口类
 * 
 * 提供了加载和保存 .png、.ppm 和 .trip（自定义的压缩类型）图像的静态方法。
 */
class ImageIO {
public:
    /**
     * @brief 从文件加载图像
     * 
     * @param file_path .png 文件路径
     * @return cv::Mat 加载的图像
     */
    static cv::Mat loadPng(const std::string& file_path);

    /**
     * @brief 从文件加载三元组压缩图像
     * 具体实现在 Ppm 部分，这里仅调用 Ppm::loadPpmAsMat
     * 
     * @param file_path .ppm 文件路径
     * @return cv::Mat 加载的图像
     */
    static cv::Mat loadPpm(const std::string& file_path);

    /**
     * @brief 从文件加载三元组压缩图像
     * 
     * @param file_path .trip 文件路径
     * @return std::vector<TripletNode> 加载的三元组
     */
    static std::vector<TripletNode> loadTrip(const std::string& file_path);

    /**
     * @brief 将图像保存到文件
     * 
     * @param file_path .png 文件路径
     * @param img 要保存的图像
     * @return true 保存成功
     * @return false 保存失败
     */
    static bool savePng(const std::string& file_path, const cv::Mat& img);

    /**
     * @brief 将图像保存到文件
     * 具体实现在 Ppm 部分，这里仅调用 Ppm::saveNatAsPpm
     * 
     * @param file_path .ppm 文件路径
     * @param img 要保存的图像
     * @return true 保存成功
     * @return false 保存失败
     */
    static bool savePpm(const std::string& file_path, const cv::Mat& img);

    /**
     * @brief 将图像保存到文件
     * 
     * @param file_path .trip 文件路径
     * @param triplets 要保存的三元组
     * @return true 保存成功
     * @return false 保存失败
     */
    static bool saveTrip(const std::string& file_path, const std::vector<TripletNode>& triplets);


    // =========================================================
    // Node.js 互操作预留接口 (Buffer I/O)
    // =========================================================

    /**
    * @brief [预留] 从内存缓冲区解码图像 (供 Node.js 调用)。
    * @param data 指向 buffer 数据的指针。
    * @param size buffer 数据长度。
    * @return cv::Mat 解码后的图像。
    */
    static cv::Mat DecodeFromBuffer(const uint8_t* data, size_t size);

    /**
    * @brief [预留] 将图像编码到内存缓冲区 (供 Node.js 调用)。
    * @param img 输入图像。
    * @param format 编码格式
    * @param[out] buffer 输出的二进制数据。
    * @return true 成功，false 失败。
    */
    static bool EncodeToBuffer(const cv::Mat& img, const std::string& format,
                                std::vector<uint8_t>* buffer);
};