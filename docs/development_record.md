### 项目初始化

使用 `electron-vite` 进行初始化，实际使用时发现直接用
```bash
npm create @quick-start/electron@latest my-app -- --template react-ts
```
初始化时，运行 `npm run dev` 指令时没有窗口弹出，而 5173 端口有东西，原因不明。

下面的初始化方式可以正常运行:
```bash
npm create electron-vite@latest .
npm install
npm install @mui/material @emotion/react @emotion/styled @mui/icons-material
npm run dev
```

### 默认字体和图表库

安装默认字体
```bash
npm install @fontsource/roboto
```
安装之后可以在文件导入
```bash
import '@fontsource/roboto/300.css';
import '@fontsource/roboto/400.css';
import '@fontsource/roboto/500.css';
import '@fontsource/roboto/700.css';
```

安装图标库
```bash
npm install @mui/icons-material
```

### C++ 核心计算模块实现
配置 `CMakeLists.txt`

用 `Google` 代码风格编码，`Doxygen` 风格注释

cpp核心计算模块分成这几个部分：
1. 读取与写入存储：读取`png（opencv）`和`ppm（手写）`成`cv::Mat`，然后可以返回给其他模块或者输出（输出为`ppm`或`png`到指定目录）。压缩生成的东西也要想办法存储。注意要为后面传送给node进程预留一些功能（输出成`buffer`给`node`）
2. 三元组相关：包含三元组的定义及实现，用通道数变量来统一彩色图与灰度图。包含统计背景色，Mat转为三元组和将三元组转为Mat等
3. 图像压缩和解压： 调用三元组模块实现解压和压缩。
4. 图像处理：彩色图变为灰度图（用经验公式）和图像尺寸缩放（手写双线性插值）
5. 各种单元测试和总体测试（放在另外一个`test`文件夹，不放`src`）

```
cpp/
├── CMakeLists.txt           # 主构建文件
├── src/
│   ├── data_structure/      # 三元组数据结构
│   │   ├── Triplet.h        # 三元组定义及背景色统计、Mat 和三元组互转等
│   │   └── Triplet.cc
│   ├── io/                  # I/O 模块
│   │   ├── ImageIO.h        # 统一图像读写接口 (含 Node Buffer 预留)
│   │   ├── ImageIO.cc
│   │   ├── Ppm.h            # 手写 PPM 读写实现，供 ImageIO 使用
│   │   └── Ppm.cc
│   ├── codec/               # 压缩/解压模块
│   │   ├── Compressor.h     # .trip 文件编解码
│   │   └── Compressor.cc
│   └── imgproc/             # 图像处理模块
│       ├── Processor.h      # 灰度化、缩放
│       └── Processor.cc
└── test/                    # 测试模块
    ├── CMakeLists.txt
    ├── test_main.cc         # 集成测试入口
    ├── unit_io.cc           # I/O 单元测试
    ├── unit_imgproc.cc      # 图像处理单元测试
    └── unit_codec.cc        # 压缩单元测试
```