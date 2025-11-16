# 图像处理器作业

某个课程的作业，在 `Debian13` 下开发。实现了基于三元组的稀疏矩阵图像压缩算法、PNG/PPM 格式支持、以及灰度转换、缩放等图像处理功能。
核心运算实现用 `C++` 并封装成 `node.js` 原生插件。
前端使用 `Electron + React + Mui`， 后端 `node.js` 调用 `C++` 封装的原生插件并处理业务逻辑。

### 项目结构

```
DataStructureProject2/
├── src/                          # React 前端代码
│   ├── App.tsx                   # 主应用组件
│   ├── main.tsx                  # 入口文件
│   └── assets/                   # 静态资源
├── electron/                     # Electron 主进程
│   ├── main.ts                   # 主窗口与 IPC 处理
│   └── preload.ts                # 预加载脚本（IPC 隔离）
├── cpp/                          # C++ 核心实现
│   ├── CMakeLists.txt            # CMake 配置
│   ├── src/
│   │   ├── ImageProcessor.h      # 图像处理类（OpenCV 封装）
│   │   └── ImageProcessor.cc
│   ├── src/io/
│   │   ├── Ppm.h                 # PPM 格式 I/O
│   │   └── Ppm.cc
│   ├── src/data_structure/
│   │   ├── Triplet.h             # 三元组数据结构
│   │   └── Triplet.cc
│   └── test/                     # C++ 单元测试
├── dist/                         # Vite 构建输出（前端）
├── dist-electron/                # Electron 编译输出
├── vite.config.ts                # Vite 配置
├── tsconfig.json                 # TypeScript 配置
├── package.json                  # 项目依赖
└── README.md                     # REAENE
```

### 

#### 前置要求
- **Node.js** >= 16.0
- **npm** >= 8.0（或 yarn/pnpm）
- **C++ 编译工具**：
  - **Windows**：Visual Studio 2019+ 或 MinGW
  - **macOS**：Xcode 命令行工具（`xcode-select --install`）
  - **Linux**：gcc/g++ >= 9.0 和 CMake >= 3.10

#### git clone 到本地

```bash
git init
git clone 本仓库地址
```

#### 安装依赖
`cd` 到项目根目录。

```bash
# 安装 Node.js 依赖
npm install

# 编译 C++ 原生模块（自动通过 node-gyp）
npm run build:native
```

#### 开发模式运行
在根目录下

```bash
npm run dev
```
