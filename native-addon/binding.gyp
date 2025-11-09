{
  "targets": [
    {
      "target_name": "project2_addon",
      "cflags!": [ "-fno-exceptions" ],
      "cflags_cc!": [ "-fno-exceptions", "-fno-rtti" ],
      "defines": [ "NAPI_CPP_EXCEPTIONS" ],
      
      "sources": [
        "src/main.cc",
        # 这里需要显式列出你 C++ 核心模块的所有源文件。
        # node-gyp 对跨目录通配符的支持有限，建议手动列出，虽然繁琐但最稳妥。
        "../cpp/src/data_structure/Triplet.cc",
        "../cpp/src/io/ImageIO.cc",
        "../cpp/src/io/Ppm.cc",
        "../cpp/src/codec/Compressor.cc",
        "../cpp/src/imgproc/ImageProcessor.cc"
      ],
      
      "include_dirs": [
        # 引入 Node-API 头文件
        "<!@(node -p \"require('node-addon-api').include\")",
        # 引入你自己的 C++ 核心头文件目录
        "../cpp/src"
      ],

      # Linux 的配置条件
      "conditions": [
        ['OS=="linux"', {
          # 1. 编译参数 (cflags_cc)
          # -std=c++17: 开启 C++17 支持
          # -fexceptions: 开启异常支持 (OpenCV 和 N-API 都需要)
          # -frtti: 开启运行时类型信息 (OpenCV 的某些高级功能需要)
          # pkg-config --cflags opencv4: 自动获取 OpenCV 的头文件路径 (-I/usr/include/opencv4 ...)
          "cflags_cc": [
            "-std=c++17",
            "-fexceptions",
            "-frtti",
            "<!@(pkg-config --cflags opencv4)"
          ],
          
          # 2. 链接参数 (libraries)
          # pkg-config --libs opencv4: 自动获取 OpenCV 的库文件路径和链接选项 (-L/usr/lib ... -lopencv_core ...)
          "libraries": [
            "<!@(pkg-config --libs opencv4)"
          ],

          # 某些较新的编译器可能会报关于异常的警告，可以屏蔽掉
          "cflags_cc!": [ "-fno-exceptions", "-fno-rtti" ]
        }]
      ]
    }
  ]
}