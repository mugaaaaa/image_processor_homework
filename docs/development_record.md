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