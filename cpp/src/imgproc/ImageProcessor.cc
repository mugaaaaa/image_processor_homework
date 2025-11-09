#include "ImageProcessor.h"
#include <cmath>

static inline uint8_t clamp255(int v) { return static_cast<uint8_t>(v < 0 ? 0 : (v > 255 ? 255 : v)); }

cv::Mat Processor::ToGray(const cv::Mat& input) {
    if (input.empty()) return cv::Mat();
    if (input.channels() != 3) return cv::Mat();
    cv::Mat gray(input.rows, input.cols, CV_8UC1);
    for (int r = 0; r < input.rows; ++r) {
        const cv::Vec3b* inrow = input.ptr<cv::Vec3b>(r);
        uint8_t* outrow = gray.ptr<uint8_t>(r);
        for (int c = 0; c < input.cols; ++c) {
            const cv::Vec3b& bgr = inrow[c];
            // Gray = 0.299*R + 0.587*G + 0.114*B ; OpenCV uses BGR order
            int g = static_cast<int>(0.299 * bgr[2] + 0.587 * bgr[1] + 0.114 * bgr[0]);
            outrow[c] = clamp255(g);
        }
    }
    return gray;
}

cv::Mat Processor::Resize(const cv::Mat& input, int new_width, int new_height) {
    if (input.empty() || new_width <= 0 || new_height <= 0) return cv::Mat();
    int ch = input.channels();
    cv::Mat out(new_height, new_width, ch == 1 ? CV_8UC1 : CV_8UC3);

    double scale_x = static_cast<double>(input.cols) / new_width;
    double scale_y = static_cast<double>(input.rows) / new_height;

    for (int y = 0; y < new_height; ++y) {
        double src_y = (y + 0.5) * scale_y - 0.5;
        int y0 = static_cast<int>(floor(src_y));
        int y1 = y0 + 1;
        double wy = src_y - y0;
        if (y0 < 0) { y0 = 0; y1 = 0; wy = 0; }
        if (y1 >= input.rows) { y1 = input.rows - 1; y0 = y1; wy = 0; }

        for (int x = 0; x < new_width; ++x) {
            double src_x = (x + 0.5) * scale_x - 0.5;
            int x0 = static_cast<int>(floor(src_x));
            int x1 = x0 + 1;
            double wx = src_x - x0;
            if (x0 < 0) { x0 = 0; x1 = 0; wx = 0; }
            if (x1 >= input.cols) { x1 = input.cols - 1; x0 = x1; wx = 0; }

            if (ch == 1) {
                const uint8_t Q00 = input.at<uint8_t>(y0, x0);
                const uint8_t Q10 = input.at<uint8_t>(y0, x1);
                const uint8_t Q01 = input.at<uint8_t>(y1, x0);
                const uint8_t Q11 = input.at<uint8_t>(y1, x1);
                double v = (1 - wx) * (1 - wy) * Q00 + wx * (1 - wy) * Q10 + (1 - wx) * wy * Q01 + wx * wy * Q11;
                out.at<uint8_t>(y, x) = clamp255(static_cast<int>(std::round(v)));
            } else {
                const cv::Vec3b Q00 = input.at<cv::Vec3b>(y0, x0);
                const cv::Vec3b Q10 = input.at<cv::Vec3b>(y0, x1);
                const cv::Vec3b Q01 = input.at<cv::Vec3b>(y1, x0);
                const cv::Vec3b Q11 = input.at<cv::Vec3b>(y1, x1);
                cv::Vec3d v = (1 - wx) * (1 - wy) * cv::Vec3d(Q00) + wx * (1 - wy) * cv::Vec3d(Q10) + (1 - wx) * wy * cv::Vec3d(Q01) + wx * wy * cv::Vec3d(Q11);
                cv::Vec3b vb(static_cast<uint8_t>(std::round(v[0])), static_cast<uint8_t>(std::round(v[1])), static_cast<uint8_t>(std::round(v[2])));
                out.at<cv::Vec3b>(y, x) = vb;
            }
        }
    }
    return out;
}