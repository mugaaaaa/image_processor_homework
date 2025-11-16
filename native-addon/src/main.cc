/**
 * @file main.cc
 * @author Runhui Mo (github.com/mugaaaaa)
 * @brief Node-API 绑定主文件
 * @version 0.1
 * @date 2025-11-11
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include <napi.h>
#include <opencv2/opencv.hpp>
#include <cstring>
#include "../../cpp/src/io/image_io.h"
#include "../../cpp/src/codec/compressor.h"
#include "../../cpp/src/imgproc/image_processor.h"

/**
 * @brief 创建 Napi::Error 异常
 * 
 * @param env Node-API 环境
 * @param msg 异常消息
 * @return Napi::Error 异常对象
 */
static inline Napi::Error MakeError(Napi::Env env, const std::string& msg) {
    return Napi::Error::New(env, msg);
}

/**
 * @brief 把 ImageIO::LoadPng 包装为 Node-API 函数
 * 
 * @details 加载 PNG 图像并返回包含图像数据的对象，格式为：
 * { width, height, channels, data: Buffer }
 * 
 * @param info Node-API 回调信息
 * @return Napi::Value 包含图像数据的对象
 */
static Napi::Value LoadPngWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // 检查参数
    if (info.Length() != 1 || !info[0].IsString()) {
    throw MakeError(env, "loadPng(filePath) expects a string path");
    }

    // 调用 ImageIO::LoadPng 加载图像
    std::string path = info[0].As<Napi::String>().Utf8Value();
    cv::Mat img = ImageIO::LoadPng(path);
    if (img.empty()) {
    throw MakeError(env, "Failed to load PNG: " + path);
    }

    // 构造 out 并返回
    size_t elemSize = img.elemSize();
    size_t byteLen = static_cast<size_t>(img.rows) * img.cols * elemSize;
    Napi::Object out = Napi::Object::New(env);
    out.Set("width", Napi::Number::New(env, img.cols));
    out.Set("height", Napi::Number::New(env, img.rows));
    out.Set("channels", Napi::Number::New(env, img.channels()));
    Napi::Buffer<uint8_t> buf = Napi::Buffer<uint8_t>::New(env, byteLen); // Buffer 存储图像数据
    std::memcpy(buf.Data(), img.data, byteLen);
    out.Set("data", buf);
    return out;
}

/**
 * @brief 把 ImageIO::LoadPpm 包装为 Node-API 函数
 * 
 * @details 加载 PPM 图像并返回包含图像数据的对象，格式为：
 * { width, height, channels, data: Buffer }
 * 
 * @param info Node-API 回调信息
 * @return Napi::Value 包含图像数据的对象
 */
static Napi::Value LoadPpmWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // 参数检查
    if (info.Length() != 1 || !info[0].IsString()) {
    throw MakeError(env, "loadPpm(filePath) expects a string path");
    }

    // 读取 PPM 图像
    std::string path = info[0].As<Napi::String>().Utf8Value();
    cv::Mat img = ImageIO::LoadPpm(path);
    if (img.empty()) {
    throw MakeError(env, "Failed to load PPM: " + path);
    }

    // 构造 out 并输出
    size_t elemSize = img.elemSize();
    size_t byteLen = static_cast<size_t>(img.rows) * img.cols * elemSize;
    Napi::Object out = Napi::Object::New(env);
    out.Set("width", Napi::Number::New(env, img.cols));
    out.Set("height", Napi::Number::New(env, img.rows));
    out.Set("channels", Napi::Number::New(env, img.channels()));
    Napi::Buffer<uint8_t> buf = Napi::Buffer<uint8_t>::New(env, byteLen);
    std::memcpy(buf.Data(), img.data, byteLen);
    out.Set("data", buf);
    return out;
}

/**
 * @brief 把 ImageIO::SavePng 包装为 Node-API 函数
 * 
 * @details 保存包含图像数据的对象到 PNG 文件
 * 
 * @param info Node-API 回调信息
 * @return Napi::Value 布尔值，表示保存是否成功
 */
static Napi::Value SavePngWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // 参数检查
    if (info.Length() != 5 || !info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber() || !info[3].IsNumber() || !info[4].IsBuffer()) {
    throw MakeError(env, "savePng(filePath, width, height, channels, dataBuffer)");
    }

    // 提取参数构造 img
    std::string path = info[0].As<Napi::String>().Utf8Value();
    int width = info[1].As<Napi::Number>().Int32Value();
    int height = info[2].As<Napi::Number>().Int32Value();
    int channels = info[3].As<Napi::Number>().Int32Value();
    Napi::Buffer<uint8_t> buf = info[4].As<Napi::Buffer<uint8_t>>();
    int type = (channels == 1) ? CV_8UC1 : CV_8UC3;
    cv::Mat img(height, width, type);
    size_t needed = static_cast<size_t>(width) * height * img.elemSize();
    if (buf.Length() < needed) {
    throw MakeError(env, "dataBuffer length is insufficient");
    }
    std::memcpy(img.data, buf.Data(), needed);
    
    // 调用 ImageIO::SavePng 保存 img
    bool ok = ImageIO::SavePng(path, img);
    return Napi::Boolean::New(env, ok);
}

/**
 * @brief 把 ImageIO::SavePpm 包装为 Node-API 函数
 * 
 * @details 保存包含图像数据的对象到 PPM 文件
 * 
 * @param info Node-API 回调信息
 * @return Napi::Value 布尔值，表示保存是否成功
 */
static Napi::Value SavePpmWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // 参数检查
    if (info.Length() != 5 || !info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber() || !info[3].IsNumber() || !info[4].IsBuffer()) {
    throw MakeError(env, "savePpm(filePath, width, height, channels, dataBuffer)");
    }

    // 提取参数构造 img
    std::string path = info[0].As<Napi::String>().Utf8Value();
    int width = info[1].As<Napi::Number>().Int32Value();
    int height = info[2].As<Napi::Number>().Int32Value();
    int channels = info[3].As<Napi::Number>().Int32Value();
    Napi::Buffer<uint8_t> buf = info[4].As<Napi::Buffer<uint8_t>>();
    int type = (channels == 1) ? CV_8UC1 : CV_8UC3;
    cv::Mat img(height, width, type);
    size_t needed = static_cast<size_t>(width) * height * img.elemSize();
    if (buf.Length() < needed) {
    throw MakeError(env, "dataBuffer length is insufficient");
    }
    std::memcpy(img.data, buf.Data(), needed);

    // 调用 ImageIO::SavePpm 保存 img
    bool ok = ImageIO::SavePpm(path, img);
    return Napi::Boolean::New(env, ok);
}

/**
 * @brief 把 Processor::ToGray 包装为 Node-API 函数
 * 
 * @details 把包含 BGR 图像数据的对象转换为灰度图像
 * 
 * @param info Node-API 回调信息
 * @return Napi::Value 包含灰度图像数据的对象
 */
static Napi::Value ToGrayWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // 参数检查
    if (info.Length() != 3 || !info[0].IsNumber() || !info[1].IsNumber() || !info[2].IsBuffer()) {
    throw MakeError(env, "toGray(width, height, bgrBuffer)");
    }

    // 提取图像信息
    int width = info[0].As<Napi::Number>().Int32Value();
    int height = info[1].As<Napi::Number>().Int32Value();
    Napi::Buffer<uint8_t> buf = info[2].As<Napi::Buffer<uint8_t>>();
    cv::Mat input(height, width, CV_8UC3);
    size_t needed = static_cast<size_t>(width) * height * input.elemSize();
    if (buf.Length() < needed) {
    throw MakeError(env, "bgrBuffer length is insufficient");
    }
    std::memcpy(input.data, buf.Data(), needed);

    // 调用 Processor::ToGray 转换为灰度图像
    cv::Mat gray = Processor::ToGray(input);

    // 返回数据
    Napi::Object out = Napi::Object::New(env);
    out.Set("width", Napi::Number::New(env, gray.cols));
    out.Set("height", Napi::Number::New(env, gray.rows));
    out.Set("channels", Napi::Number::New(env, 1));
    Napi::Buffer<uint8_t> gbuf = Napi::Buffer<uint8_t>::New(env, static_cast<size_t>(gray.rows) * gray.cols);
    std::memcpy(gbuf.Data(), gray.data, static_cast<size_t>(gray.rows) * gray.cols);
    out.Set("data", gbuf);
    return out;
}

/**
 * @brief 把 Processor::Resize 包装为 Node-API 函数
 * 
 * @details 把包含图像数据的对象按指定比例缩放
 * 
 * @param info Node-API 回调信息
 * @return Napi::Value 包含缩放后图像数据的对象
 */
static Napi::Value ResizeWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // 参数检查
    if (info.Length() != 6 || !info[0].IsNumber() || !info[1].IsNumber() || !info[2].IsNumber() || !info[3].IsNumber() || !info[4].IsNumber() || !info[5].IsBuffer()) {
    throw MakeError(env, "resize(width, height, channels, newWidth, newHeight, dataBuffer)");
    }

    // 提取图像信息
    int width = info[0].As<Napi::Number>().Int32Value();
    int height = info[1].As<Napi::Number>().Int32Value();
    int channels = info[2].As<Napi::Number>().Int32Value();
    int newW = info[3].As<Napi::Number>().Int32Value();
    int newH = info[4].As<Napi::Number>().Int32Value();
    Napi::Buffer<uint8_t> buf = info[5].As<Napi::Buffer<uint8_t>>();
    int type = (channels == 1) ? CV_8UC1 : CV_8UC3;
    cv::Mat input(height, width, type);
    size_t needed = static_cast<size_t>(width) * height * input.elemSize();
    if (buf.Length() < needed) {
    throw MakeError(env, "dataBuffer length is insufficient");
    }
    std::memcpy(input.data, buf.Data(), needed);

    // 调用 Processor::Resize 缩放图像
    cv::Mat outImg = Processor::Resize(input, newW, newH);

    // 构造 out 并返回数据
    Napi::Object out = Napi::Object::New(env);
    out.Set("width", Napi::Number::New(env, outImg.cols));
    out.Set("height", Napi::Number::New(env, outImg.rows));
    out.Set("channels", Napi::Number::New(env, outImg.channels()));
    Napi::Buffer<uint8_t> obuf = Napi::Buffer<uint8_t>::New(env, static_cast<size_t>(outImg.rows) * outImg.cols * outImg.elemSize());
    std::memcpy(obuf.Data(), outImg.data, static_cast<size_t>(outImg.rows) * outImg.cols * outImg.elemSize());
    out.Set("data", obuf);
    return out;
}

/**
 * @brief 把 Compressor::Save 包装为 Node-API 函数
 * 
 * @details 把包含图像数据的对象保存为 .trip 格式的压缩文件
 * 
 * @param info Node-API 回调信息
 * @return Napi::Value 布尔值，表示保存是否成功
 */
static Napi::Value CompressorSaveWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // 参数检查
    if (info.Length() != 5 || !info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber() || !info[3].IsNumber() || !info[4].IsBuffer()) {
    throw MakeError(env, "compressorSave(filePath, width, height, channels, dataBuffer)");
    }

    // 提取图像信息
    std::string path = info[0].As<Napi::String>().Utf8Value();
    int width = info[1].As<Napi::Number>().Int32Value();
    int height = info[2].As<Napi::Number>().Int32Value();
    int channels = info[3].As<Napi::Number>().Int32Value();
    Napi::Buffer<uint8_t> buf = info[4].As<Napi::Buffer<uint8_t>>();
    int type = (channels == 1) ? CV_8UC1 : CV_8UC3;
    cv::Mat img(height, width, type);
    size_t needed = static_cast<size_t>(width) * height * img.elemSize();
    if (buf.Length() < needed) {
    throw MakeError(env, "dataBuffer length is insufficient");
    }
    std::memcpy(img.data, buf.Data(), needed);

    // 调用 Compressor::Save 保存图像
    bool ok = Compressor::Save(path, img);
    return Napi::Boolean::New(env, ok);
}

/**
 * @brief 把 Compressor::Load 包装为 Node-API 函数
 * 
 * @details 从压缩文件加载图像数据
 * 
 * @param info Node-API 回调信息
 * @return Napi::Value 包含图像数据的对象
 */
static Napi::Value CompressorLoadWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // 参数检查
    if (info.Length() != 1 || !info[0].IsString()) {
    throw MakeError(env, "compressorLoad(filePath)");
    }

    // 读取图像信息
    std::string path = info[0].As<Napi::String>().Utf8Value();
    cv::Mat img = Compressor::Load(path);
    if (img.empty()) {
    throw MakeError(env, "Failed to load .trip: " + path);
    }

    // 构造 out 并返回数据
    size_t byteLen = static_cast<size_t>(img.rows) * img.cols * img.elemSize();
    Napi::Object out = Napi::Object::New(env);
    out.Set("width", Napi::Number::New(env, img.cols));
    out.Set("height", Napi::Number::New(env, img.rows));
    out.Set("channels", Napi::Number::New(env, img.channels()));
    Napi::Buffer<uint8_t> buf = Napi::Buffer<uint8_t>::New(env, byteLen);
    std::memcpy(buf.Data(), img.data, byteLen);
    out.Set("data", buf);
    return out;
}

/**
 * @brief 把 ImageIO::SaveTrip 包装为 Node-API 函数
 * 
 * @details 把包含三元组数据的对象保存到压缩文件
 * 
 * @param info Node-API 回调信息
 * @return Napi::Value 布尔值，表示保存是否成功
 */
static Napi::Value SaveTripWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // 参数检查
    if (info.Length() != 6 || !info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber() || !info[3].IsNumber() || !info[4].IsBuffer() || !info[5].IsArray()) {
    throw MakeError(env, "saveTrip(filePath, width, height, channels, bgColorBuffer[3], tripletsArray)");
    }

    // 提取图像信息
    std::string path = info[0].As<Napi::String>().Utf8Value();
    int width = info[1].As<Napi::Number>().Int32Value();
    int height = info[2].As<Napi::Number>().Int32Value();
    int channels = info[3].As<Napi::Number>().Int32Value();
    Napi::Buffer<uint8_t> bgBuf = info[4].As<Napi::Buffer<uint8_t>>();
    if (bgBuf.Length() < 3) {
    throw MakeError(env, "bgColorBuffer must have length >= 3");
    }

    // 背景颜色
    uint8_t bg[3] = { bgBuf[0], bgBuf[1], bgBuf[2] };

    // .trip 文件头
    Napi::Array arr = info[5].As<Napi::Array>();

    // 初始化三元组数据，预留空间
    std::vector<TripletNode> triplets;
    triplets.reserve(arr.Length());

    // 遍历数组，提取三元组数据
    for (uint32_t i = 0; i < arr.Length(); ++i) {
        Napi::Value v = arr.Get(i);
        if (!v.IsObject()) continue;
        Napi::Object o = v.As<Napi::Object>();
        TripletNode t{};
        t.row_ = o.Get("row").As<Napi::Number>().Int32Value();
        t.col_ = o.Get("col").As<Napi::Number>().Int32Value();
        Napi::Array val = o.Get("val").As<Napi::Array>();
        t.val_[0] = static_cast<uint8_t>(val.Get((uint32_t)0).As<Napi::Number>().Int32Value());
        t.val_[1] = static_cast<uint8_t>(val.Get((uint32_t)1).As<Napi::Number>().Int32Value());
        t.val_[2] = static_cast<uint8_t>(val.Get((uint32_t)2).As<Napi::Number>().Int32Value());
        triplets.push_back(t);
    }

    // 调用 ImageIO::SaveTrip 保存图像
    bool ok = ImageIO::SaveTrip(path, width, height, channels, bg, triplets);
    return Napi::Boolean::New(env, ok);
}

/**
 * @brief 初始化 Node-API 模块
 * 
 * @details 注册所有 Node-API 函数
 * 
 * @param env Node-API 环境
 * @param exports 导出对象
 * @return Napi::Object 导出对象
 */
static Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("loadPng", Napi::Function::New(env, LoadPngWrapped));
    exports.Set("loadPpm", Napi::Function::New(env, LoadPpmWrapped));
    exports.Set("savePng", Napi::Function::New(env, SavePngWrapped));
    exports.Set("savePpm", Napi::Function::New(env, SavePpmWrapped));
    exports.Set("toGray", Napi::Function::New(env, ToGrayWrapped));
    exports.Set("resize", Napi::Function::New(env, ResizeWrapped));
    exports.Set("saveTrip", Napi::Function::New(env, SaveTripWrapped));
    exports.Set("compressorSave", Napi::Function::New(env, CompressorSaveWrapped));
    exports.Set("compressorLoad", Napi::Function::New(env, CompressorLoadWrapped));
    return exports;
}

NODE_API_MODULE(project2_addon, Init)
