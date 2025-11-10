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
    // // 去掉窗口原生边框/工具栏（frame: false），并隐藏菜单栏（autoHideMenuBar: true）
    // // 注意：去掉 frame 后，窗口将没有系统标题栏，需要在渲染层添加可拖拽区域（-webkit-app-region: drag）
    // frame: false,
    // autoHideMenuBar: true,
    // // macOS 专用：隐藏原生标题栏，配合 frameless 使用可以得到更自然的外观
    // titleBarStyle: 'hidden',
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

  // 移除应用菜单（使窗口上方不显示菜单栏）。在某些平台上还可以用 win.setMenuBarVisibility(false)
  try {
    Menu.setApplicationMenu(null)
    win.setMenuBarVisibility(false)
  } catch (e) {
    // ignore if Menu API not available for some reason
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

// -------------------------------------------------------
// Native addon wiring via IPC
// -------------------------------------------------------
// Resolve native addon from project root
// Allow `any` here because native addon exports are dynamic
// eslint-disable-next-line @typescript-eslint/no-explicit-any
let native: any
try {
  native = require(path.join(process.env.APP_ROOT, 'native-addon'))
} catch (e) {
  console.error('Failed to load native addon. Please build it in native-addon.', e)
}

// Helpers to convert data
const toBuffer = (u8: Uint8Array) => Buffer.from(u8.buffer, u8.byteOffset, u8.byteLength)

ipcMain.handle('native:loadPpm', async (_event, filePath: string) => {
  if (!native) throw new Error('native addon not loaded')
  return native.loadPpm(filePath)
})

ipcMain.handle('native:loadPng', async (_event, filePath: string) => {
  if (!native) throw new Error('native addon not loaded')
  return native.loadPng(filePath)
})

ipcMain.handle('native:savePpm', async (_event, filePath: string, img: { width: number, height: number, channels: number, data: Uint8Array }) => {
  if (!native) throw new Error('native addon not loaded')
  return native.savePpm(filePath, img.width, img.height, img.channels, toBuffer(img.data))
})

ipcMain.handle('native:savePng', async (_event, filePath: string, img: { width: number, height: number, channels: number, data: Uint8Array }) => {
  if (!native) throw new Error('native addon not loaded')
  return native.savePng(filePath, img.width, img.height, img.channels, toBuffer(img.data))
})

ipcMain.handle('native:toGray', async (_event, img: { width: number, height: number, data: Uint8Array }) => {
  if (!native) throw new Error('native addon not loaded')
  return native.toGray(img.width, img.height, toBuffer(img.data))
})

ipcMain.handle('native:resize', async (_event, img: { width: number, height: number, channels: number, data: Uint8Array }, newW: number, newH: number) => {
  if (!native) throw new Error('native addon not loaded')
  return native.resize(img.width, img.height, img.channels, newW, newH, toBuffer(img.data))
})

ipcMain.handle('native:compressorSave', async (_event, filePath: string, img: { width: number, height: number, channels: number, data: Uint8Array }) => {
  if (!native) throw new Error('native addon not loaded')
  return native.compressorSave(filePath, img.width, img.height, img.channels, toBuffer(img.data))
})

ipcMain.handle('native:compressorLoad', async (_event, filePath: string) => {
  if (!native) throw new Error('native addon not loaded')
  return native.compressorLoad(filePath)
})

// -------------------------------------------------------
// File dialogs (open/save) for images and .trip
// -------------------------------------------------------
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

ipcMain.handle('dialog:savePng', async () => {
  const result = await dialog.showSaveDialog(win!, {
    filters: [{ name: 'PNG Image', extensions: ['png'] }],
    defaultPath: 'image.png'
  })
  return result.canceled ? null : result.filePath
})

ipcMain.handle('dialog:savePpm', async () => {
  const result = await dialog.showSaveDialog(win!, {
    filters: [{ name: 'PPM Image', extensions: ['ppm'] }],
    defaultPath: 'image.ppm'
  })
  return result.canceled ? null : result.filePath
})

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

ipcMain.handle('dialog:saveTrip', async () => {
  const result = await dialog.showSaveDialog(win!, {
    filters: [{ name: 'Trip Files', extensions: ['trip'] }],
    defaultPath: 'image.trip'
  })
  return result.canceled ? null : result.filePath
})

// Unified save dialog for images (PNG or PPM)
ipcMain.handle('dialog:saveImage', async () => {
  const result = await dialog.showSaveDialog(win!, {
    filters: [
      { name: 'PNG Image', extensions: ['png'] },
      { name: 'PPM Image', extensions: ['ppm'] },
    ],
    defaultPath: 'image.png'
  })
  return result.canceled ? null : result.filePath
})
