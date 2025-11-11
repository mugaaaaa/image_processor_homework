/* eslint-disable @typescript-eslint/no-explicit-any */
/**
 * @file main.ts
 * @author Runhui Mo (github.com/mugaaaaa)
 * @brief Electron 主进程入口文件
 *
 * 包含脚手架自动生成的部分代码，以及与原生插件和文件对话框相关的 IPC 处理逻辑。
 * 
 * @version 0.1
 * @date 2025-11-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

import { app, BrowserWindow, ipcMain, dialog, Menu } from 'electron'
import { createRequire } from 'node:module'
import { fileURLToPath } from 'node:url'
import path from 'node:path'

const require = createRequire(import.meta.url)
const __dirname = path.dirname(fileURLToPath(import.meta.url))

// The built directory structure
//
// ├─┬─┬ dist
// │ │ └── index.html
// │ │
// │ ├─┬ dist-electron
// │ │ ├── main.js
// │ │ └── preload.mjs
// │
process.env.APP_ROOT = path.join(__dirname, '..')

// 🚧 Use ['ENV_NAME'] avoid vite:define plugin - Vite@2.x
export const VITE_DEV_SERVER_URL = process.env['VITE_DEV_SERVER_URL']
export const MAIN_DIST = path.join(process.env.APP_ROOT, 'dist-electron')
export const RENDERER_DIST = path.join(process.env.APP_ROOT, 'dist')

process.env.VITE_PUBLIC = VITE_DEV_SERVER_URL ? path.join(process.env.APP_ROOT, 'public') : RENDERER_DIST

let win: BrowserWindow | null

function createWindow() {
  win = new BrowserWindow({
    width: 1024,
    height: 612,
    icon: path.join(process.env.VITE_PUBLIC, 'electron-vite.svg'),
    webPreferences: {
      preload: path.join(__dirname, 'preload.mjs'),
      contextIsolation: true,
      nodeIntegration: false,
    },
  })

  // Test active push message to Renderer-process.
  win.webContents.on('did-finish-load', () => {
    win?.webContents.send('main-process-message', (new Date).toLocaleString())
  })

  // 移除应用菜单（使窗口上方不显示菜单栏）
  try {
    Menu.setApplicationMenu(null)
    win.setMenuBarVisibility(false)
  } catch (e) {
    console.error('Failed to remove application menu', e)
  }

  if (VITE_DEV_SERVER_URL) {
    win.loadURL(VITE_DEV_SERVER_URL)
  } else {
    // win.loadFile('dist/index.html')
    win.loadFile(path.join(RENDERER_DIST, 'index.html'))
  }
}

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
    win = null
  }
})

app.on('activate', () => {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow()
  }
})

app.whenReady().then(createWindow)

// =========================================================
// 原生插件 IPC 连接
// =========================================================
let native: any
try {
  native = require(path.join(process.env.APP_ROOT, 'native-addon'))   // 从根目录加载原生插件
} catch (e) {
  console.error('Failed to load native addon. Please build it in native-addon.', e)
}

// 辅助函数
const toBuffer = (u8: Uint8Array) => Buffer.from(u8.buffer, u8.byteOffset, u8.byteLength)

// 加载 PPM 图像  
ipcMain.handle('native:loadPpm', async (_event, filePath: string) => {
  if (!native) throw new Error('native addon not loaded')
  return native.loadPpm(filePath)
})

// 加载 PNG 图像  
ipcMain.handle('native:loadPng', async (_event, filePath: string) => {
  if (!native) throw new Error('native addon not loaded')
  return native.loadPng(filePath)
})

// 保存 PPM 图像  
ipcMain.handle('native:savePpm', async (_event, filePath: string, img: { width: number, height: number, channels: number, data: Uint8Array }) => {
  if (!native) throw new Error('native addon not loaded')
  return native.savePpm(filePath, img.width, img.height, img.channels, toBuffer(img.data))
})

// 保存 PNG 图像  
ipcMain.handle('native:savePng', async (_event, filePath: string, img: { width: number, height: number, channels: number, data: Uint8Array }) => {
  if (!native) throw new Error('native addon not loaded')
  return native.savePng(filePath, img.width, img.height, img.channels, toBuffer(img.data))
})

// 转换为灰度图  
ipcMain.handle('native:toGray', async (_event, img: { width: number, height: number, data: Uint8Array }) => {
  if (!native) throw new Error('native addon not loaded')
  return native.toGray(img.width, img.height, toBuffer(img.data))
})

// 缩放
ipcMain.handle('native:resize', async (_event, img: { width: number, height: number, channels: number, data: Uint8Array }, newW: number, newH: number) => {
  if (!native) throw new Error('native addon not loaded')
  return native.resize(img.width, img.height, img.channels, newW, newH, toBuffer(img.data))
})

// 压缩为 .trip 文件并保存
ipcMain.handle('native:compressorSave', async (_event, filePath: string, img: { width: number, height: number, channels: number, data: Uint8Array }) => {
  if (!native) throw new Error('native addon not loaded')
  return native.compressorSave(filePath, img.width, img.height, img.channels, toBuffer(img.data))
})

// 从 .trip 文件加载图像
ipcMain.handle('native:compressorLoad', async (_event, filePath: string) => {
  if (!native) throw new Error('native addon not loaded')
  return native.compressorLoad(filePath)
})

// =========================================================
// 加载图像文件对话框
// =========================================================

// 打开图像文件对话框
ipcMain.handle('dialog:openImage', async () => {
  const result = await dialog.showOpenDialog(win!, {
    properties: ['openFile'],
    filters: [
      { name: 'Images', extensions: ['png', 'ppm'] },
      { name: 'All Files', extensions: ['*'] }
    ]
  })
  return result.canceled ? null : result.filePaths[0]
})

// 打开 PNG 图像保存对话框
ipcMain.handle('dialog:savePng', async () => {
  const result = await dialog.showSaveDialog(win!, {
    filters: [{ name: 'PNG Image', extensions: ['png'] }],
    defaultPath: 'image.png'
  })
  return result.canceled ? null : result.filePath
})

// 打开 PPM 图像保存对话框
ipcMain.handle('dialog:savePpm', async () => {
  const result = await dialog.showSaveDialog(win!, {
    filters: [{ name: 'PPM Image', extensions: ['ppm'] }],
    defaultPath: 'image.ppm'
  })
  return result.canceled ? null : result.filePath
})

// 打开 Trip 文件对话框
ipcMain.handle('dialog:openTrip', async () => {
  const result = await dialog.showOpenDialog(win!, {
    properties: ['openFile'],
    filters: [
      { name: 'Trip Files', extensions: ['trip'] },
      { name: 'All Files', extensions: ['*'] }
    ]
  })
  return result.canceled ? null : result.filePaths[0]
})

// 打开 Trip 文件保存对话框
ipcMain.handle('dialog:saveTrip', async () => {
  const result = await dialog.showSaveDialog(win!, {
    filters: [{ name: 'Trip Files', extensions: ['trip'] }],
    defaultPath: 'image.trip'
  })
  return result.canceled ? null : result.filePath
})

// 图像保存对话框
// ipcMain.handle('dialog:saveImage', async () => {
//   const result = await dialog.showSaveDialog(win!, {
//     filters: [
//       { name: 'PNG Image', extensions: ['png'] },
//       { name: 'PPM Image', extensions: ['ppm'] },
//     ],
//     defaultPath: 'image.png'
//   })
//   return result.canceled ? null : result.filePath
// })
