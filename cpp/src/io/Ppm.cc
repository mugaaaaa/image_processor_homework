#include "Ppm.h"
#include <fstream>
#include <sstream>
#include <string>

static bool readToken(std::istream& is, std::string& tok) {
    tok.clear();
    char ch;
    // skip whitespace and comments
    while (is.get(ch)) {
        if (ch == '#') { // comment line
            std::string dummy;
            std::getline(is, dummy);
            continue;
        }
        if (!std::isspace(static_cast<unsigned char>(ch))) {
            tok.push_back(ch);
            break;
        }
    }
    while (is.get(ch)) {
        if (std::isspace(static_cast<unsigned char>(ch))) break;
        tok.push_back(ch);
    }
    return !tok.empty();
}

cv::Mat Ppm::loadPpmAsMat(const std::string& file_path) {
    std::ifstream ifs(file_path);
    if (!ifs) return cv::Mat();

    std::string magic; if (!readToken(ifs, magic)) return cv::Mat();
    if (magic != "P2" && magic != "P3") return cv::Mat();
    std::string wtok, htok, maxtok;
    if (!readToken(ifs, wtok)) return cv::Mat();
    if (!readToken(ifs, htok)) return cv::Mat();
    if (!readToken(ifs, maxtok)) return cv::Mat();
    int width = std::stoi(wtok);
    int height = std::stoi(htok);
    int maxv = std::stoi(maxtok);
    if (width <= 0 || height <= 0 || maxv <= 0) return cv::Mat();

    if (magic == "P2") {
        cv::Mat img(height, width, CV_8UC1);
        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
                std::string vtok; if (!readToken(ifs, vtok)) return cv::Mat();
                int v = std::stoi(vtok);
                if (v < 0) v = 0; if (v > maxv) v = maxv;
                uint8_t u = static_cast<uint8_t>(v * 255 / maxv);
                img.at<uint8_t>(r, c) = u;
            }
        }
        return img;
    } else { // P3
        cv::Mat img(height, width, CV_8UC3);
        for (int r = 0; r < height; ++r) {
            for (int c = 0; c < width; ++c) {
                std::string rtok, gtok, btok;
                if (!readToken(ifs, rtok)) return cv::Mat();
                if (!readToken(ifs, gtok)) return cv::Mat();
                if (!readToken(ifs, btok)) return cv::Mat();
                int R = std::stoi(rtok);
                int G = std::stoi(gtok);
                int B = std::stoi(btok);
                if (R < 0) R = 0; if (R > maxv) R = maxv;
                if (G < 0) G = 0; if (G > maxv) G = maxv;
                if (B < 0) B = 0; if (B > maxv) B = maxv;
                // Convert to OpenCV BGR
                img.at<cv::Vec3b>(r, c) = cv::Vec3b(
                    static_cast<uint8_t>(B * 255 / maxv),
                    static_cast<uint8_t>(G * 255 / maxv),
                    static_cast<uint8_t>(R * 255 / maxv));
            }
        }
        return img;
    }
}

bool Ppm::saveNatAsPpm(const std::string& file_path, const cv::Mat& img) {
    std::ofstream ofs(file_path);
    if (!ofs) return false;
    int width = img.cols, height = img.rows;
    int channels = img.channels();
    int maxv = 255;
    if (channels == 1) {
        ofs << "P2\n" << width << " " << height << "\n" << maxv << "\n";
        for (int r = 0; r < height; ++r) {
            const uint8_t* rowp = img.ptr<uint8_t>(r);
            for (int c = 0; c < width; ++c) {
                ofs << static_cast<int>(rowp[c]);
                if (c + 1 < width) ofs << ' ';
            }
            ofs << '\n';
        }
        return true;
    } else if (channels == 3) {
        ofs << "P3\n" << width << " " << height << "\n" << maxv << "\n";
        for (int r = 0; r < height; ++r) {
            const cv::Vec3b* rowp = img.ptr<cv::Vec3b>(r);
            for (int c = 0; c < width; ++c) {
                const cv::Vec3b& bgr = rowp[c];
                // write as R G B
                ofs << static_cast<int>(bgr[2]) << ' '
                    << static_cast<int>(bgr[1]) << ' '
                    << static_cast<int>(bgr[0]);
                if (c + 1 < width) ofs << ' ';
            }
            ofs << '\n';
        }
        return true;
    }
    return false;
}