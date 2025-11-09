import { app, BrowserWindow, ipcMain } from 'electron'
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
