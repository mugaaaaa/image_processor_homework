// N-API addon entry: expose core C++ functions to Node.js
#include <napi.h>
#include <opencv2/opencv.hpp>
#include <cstring>
#include "../../cpp/src/io/image_io.h"
#include "../../cpp/src/codec/compressor.h"
#include "../../cpp/src/imgproc/image_processor.h"

// Utility: create Napi::Error on failure
static inline Napi::Error MakeError(Napi::Env env, const std::string& msg) {
  return Napi::Error::New(env, msg);
}

// loadPng(filePath) -> Buffer (encoded PNG) or return Mat as Uint8Array? We expose decoded Mat as Uint8Array with width/height/channels
// For simplicity and cross-compatibility, we will return an object: { width, height, channels, data: Buffer }
static Napi::Value LoadPngWrapped(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() != 1 || !info[0].IsString()) {
    throw MakeError(env, "loadPng(filePath) expects a string path");
  }
  std::string path = info[0].As<Napi::String>().Utf8Value();
  cv::Mat img = ImageIO::LoadPng(path);
  if (img.empty()) {
    throw MakeError(env, "Failed to load PNG: " + path);
  }
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

static Napi::Value LoadPpmWrapped(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() != 1 || !info[0].IsString()) {
    throw MakeError(env, "loadPpm(filePath) expects a string path");
  }
  std::string path = info[0].As<Napi::String>().Utf8Value();
  cv::Mat img = ImageIO::LoadPpm(path);
  if (img.empty()) {
    throw MakeError(env, "Failed to load PPM: " + path);
  }
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

static Napi::Value SavePngWrapped(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() != 5 || !info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber() || !info[3].IsNumber() || !info[4].IsBuffer()) {
    throw MakeError(env, "savePng(filePath, width, height, channels, dataBuffer)");
  }
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
  bool ok = ImageIO::SavePng(path, img);
  return Napi::Boolean::New(env, ok);
}

static Napi::Value SavePpmWrapped(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() != 5 || !info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber() || !info[3].IsNumber() || !info[4].IsBuffer()) {
    throw MakeError(env, "savePpm(filePath, width, height, channels, dataBuffer)");
  }
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
  bool ok = ImageIO::SavePpm(path, img);
  return Napi::Boolean::New(env, ok);
}

static Napi::Value ToGrayWrapped(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() != 3 || !info[0].IsNumber() || !info[1].IsNumber() || !info[2].IsBuffer()) {
    throw MakeError(env, "toGray(width, height, bgrBuffer)");
  }
  int width = info[0].As<Napi::Number>().Int32Value();
  int height = info[1].As<Napi::Number>().Int32Value();
  Napi::Buffer<uint8_t> buf = info[2].As<Napi::Buffer<uint8_t>>();
  cv::Mat input(height, width, CV_8UC3);
  size_t needed = static_cast<size_t>(width) * height * input.elemSize();
  if (buf.Length() < needed) {
    throw MakeError(env, "bgrBuffer length is insufficient");
  }
  std::memcpy(input.data, buf.Data(), needed);
  cv::Mat gray = Processor::ToGray(input);
  Napi::Object out = Napi::Object::New(env);
  out.Set("width", Napi::Number::New(env, gray.cols));
  out.Set("height", Napi::Number::New(env, gray.rows));
  out.Set("channels", Napi::Number::New(env, 1));
  Napi::Buffer<uint8_t> gbuf = Napi::Buffer<uint8_t>::New(env, static_cast<size_t>(gray.rows) * gray.cols);
  std::memcpy(gbuf.Data(), gray.data, static_cast<size_t>(gray.rows) * gray.cols);
  out.Set("data", gbuf);
  return out;
}

static Napi::Value ResizeWrapped(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() != 6 || !info[0].IsNumber() || !info[1].IsNumber() || !info[2].IsNumber() || !info[3].IsNumber() || !info[4].IsNumber() || !info[5].IsBuffer()) {
    throw MakeError(env, "resize(width, height, channels, newWidth, newHeight, dataBuffer)");
  }
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
  cv::Mat outImg = Processor::Resize(input, newW, newH);
  Napi::Object out = Napi::Object::New(env);
  out.Set("width", Napi::Number::New(env, outImg.cols));
  out.Set("height", Napi::Number::New(env, outImg.rows));
  out.Set("channels", Napi::Number::New(env, outImg.channels()));
  Napi::Buffer<uint8_t> obuf = Napi::Buffer<uint8_t>::New(env, static_cast<size_t>(outImg.rows) * outImg.cols * outImg.elemSize());
  std::memcpy(obuf.Data(), outImg.data, static_cast<size_t>(outImg.rows) * outImg.cols * outImg.elemSize());
  out.Set("data", obuf);
  return out;
}

static Napi::Value CompressorSaveWrapped(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() != 5 || !info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber() || !info[3].IsNumber() || !info[4].IsBuffer()) {
    throw MakeError(env, "compressorSave(filePath, width, height, channels, dataBuffer)");
  }
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
  bool ok = Compressor::Save(path, img);
  return Napi::Boolean::New(env, ok);
}

static Napi::Value CompressorLoadWrapped(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() != 1 || !info[0].IsString()) {
    throw MakeError(env, "compressorLoad(filePath)");
  }
  std::string path = info[0].As<Napi::String>().Utf8Value();
  cv::Mat img = Compressor::Load(path);
  if (img.empty()) {
    throw MakeError(env, "Failed to load .trip: " + path);
  }
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

static Napi::Value SaveTripWrapped(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  if (info.Length() != 6 || !info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber() || !info[3].IsNumber() || !info[4].IsBuffer() || !info[5].IsArray()) {
    throw MakeError(env, "saveTrip(filePath, width, height, channels, bgColorBuffer[3], tripletsArray)");
  }
  std::string path = info[0].As<Napi::String>().Utf8Value();
  int width = info[1].As<Napi::Number>().Int32Value();
  int height = info[2].As<Napi::Number>().Int32Value();
  int channels = info[3].As<Napi::Number>().Int32Value();
  Napi::Buffer<uint8_t> bgBuf = info[4].As<Napi::Buffer<uint8_t>>();
  if (bgBuf.Length() < 3) {
    throw MakeError(env, "bgColorBuffer must have length >= 3");
  }
  uint8_t bg[3] = { bgBuf[0], bgBuf[1], bgBuf[2] };
  Napi::Array arr = info[5].As<Napi::Array>();
  std::vector<TripletNode> triplets;
  triplets.reserve(arr.Length());
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
  bool ok = ImageIO::SaveTrip(path, width, height, channels, bg, triplets);
  return Napi::Boolean::New(env, ok);
}

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