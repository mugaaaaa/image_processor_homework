用 Google 代码风格编码，Doxygen 风格注释

cpp核心计算模块分成这几个部分：
1. 读取与写入存储：读取png（opencv）和ppm（手写）成cv::Mat，然后可以返回给其他模块或者输出（输出为ppm或png到指定目录）。压缩生成的东西也要想办法存储。注意要为后面传送给node进程预留一些功能（输出成buffer给node）
2. 三元组相关：包含三元组的定义及实现，用通道数变量来统一彩色图与灰度图。包含统计背景色，Mat转为三元组和将三元组转为Mat等
3. 图像压缩和解压： 调用三元组模块实现解压和压缩。
4. 图像处理：彩色图变为灰度图（用经验公式）和图像尺寸缩放（手写双线性插值）
5. 各种单元测试和总体测试（放在另外一个test文件夹，不放src）

```
cpp/
├── CMakeLists.txt           # 主构建文件
├── src/
│   ├── data_structure/                  # 三元组数据结构
│   │   ├── Triplet.h        # 三元组定义及基础转换工具
│   │   └── Triplet.cc       # 实现背景色统计、Mat转三元组等
│   ├── io/                  # I/O 模块
│   │   ├── ImageIO.h        # 统一图像读写接口 (含 Node Buffer 预留)
│   │   ├── ImageIO.cc
│   │   ├── Ppm.h            # 手写 PPM 读写实现，供 ImageIO 使用
│   │   └── Ppm.cc
│   ├── codec/               # 压缩/解压模块
│   │   ├── Compressor.h     # .trip 文件编解码
│   │   └── Compressor.cc
│   └── imgproc/             # 图像处理算法模块
│       ├── Processor.h      # 灰度化、缩放
│       └── Processor.cc
└── test/                    # 测试模块
    ├── CMakeLists.txt
    ├── test_main.cc         # 集成测试入口
    ├── unit_io.cc           # I/O 单元测试
    ├── unit_imgproc.cc         # 算法单元测试
    └── unit_codec.cc        # 压缩单元测试
```