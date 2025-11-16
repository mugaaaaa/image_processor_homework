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

### C++ 模块包装成 node 原生插件

初始化子项目
```bash
mkdir native-addon
cd native-addon
npm init -y
# 安装必要的构建工具和头文件
npm install node-addon-api bindings
npm install --save-dev node-gyp
```

目录结构
```
native-addon/
├── package.json
├── binding.gyp           # 核心构建配置
├── index.js              # JS 端入口（暴露给主进程用）
└── src/
    └── main.cc           # N-API 胶水代码入口
```

单元测试方法
```bash
cd native-addon
npm install --save-dev mocha
```
