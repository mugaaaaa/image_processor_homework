// 图像处理模块单元测试：灰度转换与双线性缩放
#include <iostream>
#include <opencv2/opencv.hpp>
#include "../src/imgproc/ImageProcessor.h"
#include "../src/io/ImageIO.h"

int test_imgproc() {
    int failed = 0;
    cv::Mat color = ImageIO::loadPpm(std::string(DATA_DIR) + "/color-block.ppm");
    if (color.empty()) { std::cerr << "[ImgProc] load color failed" << std::endl; return ++failed; }

    // ToGray
    cv::Mat gray = Processor::ToGray(color);
    if (gray.empty() || gray.channels() != 1 || gray.rows != color.rows || gray.cols != color.cols) {
        std::cerr << "[ImgProc] ToGray failed" << std::endl; ++failed;
    }

    // Resize up/down
    cv::Mat up = Processor::Resize(gray, gray.cols * 2, gray.rows * 2);
    if (up.empty() || up.rows != gray.rows * 2 || up.cols != gray.cols * 2) { std::cerr << "[ImgProc] Resize up failed" << std::endl; ++failed; }
    cv::Mat down = Processor::Resize(gray, gray.cols / 2, gray.rows / 2);
    if (down.empty() || down.rows != gray.rows / 2 || down.cols != gray.cols / 2) { std::cerr << "[ImgProc] Resize down failed" << std::endl; ++failed; }

    return failed;
}