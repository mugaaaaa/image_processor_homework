/**
 * @file main.ts
 * @author Runhui Mo (github.com/mugaaaaa)
 * @brief Electron 预加载脚本
 *
 * @details 暴露部分 IPC 和原生插件相关 API 给渲染进程使用。
 * 
 * @version 0.1
 * @date 2025-11-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

import { ipcRenderer, contextBridge } from 'electron'

// --------- Expose some API to the Renderer process ---------
contextBridge.exposeInMainWorld('ipcRenderer', {
  on(...args: Parameters<typeof ipcRenderer.on>) {
    const [channel, listener] = args
    return ipcRenderer.on(channel, (event, ...args) => listener(event, ...args))
  },
  off(...args: Parameters<typeof ipcRenderer.off>) {
    const [channel, ...omit] = args
    return ipcRenderer.off(channel, ...omit)
  },
  send(...args: Parameters<typeof ipcRenderer.send>) {
    const [channel, ...omit] = args
    return ipcRenderer.send(channel, ...omit)
  },
  invoke(...args: Parameters<typeof ipcRenderer.invoke>) {
    const [channel, ...omit] = args
    return ipcRenderer.invoke(channel, ...omit)
  },

  // You can expose other APTs you need here.
  // ...
})

// 
contextBridge.exposeInMainWorld('native', {
  loadPpm: (filePath: string) => ipcRenderer.invoke('native:loadPpm', filePath),
  loadPng: (filePath: string) => ipcRenderer.invoke('native:loadPng', filePath),
  savePpm: (filePath: string, img: { width: number, height: number, channels: number, data: Uint8Array }) => ipcRenderer.invoke('native:savePpm', filePath, img),
  savePng: (filePath: string, img: { width: number, height: number, channels: number, data: Uint8Array }) => ipcRenderer.invoke('native:savePng', filePath, img),
  toGray: (img: { width: number, height: number, data: Uint8Array }) => ipcRenderer.invoke('native:toGray', img),
  resize: (img: { width: number, height: number, channels: number, data: Uint8Array }, newW: number, newH: number) => ipcRenderer.invoke('native:resize', img, newW, newH),
  compressorSave: (filePath: string, img: { width: number, height: number, channels: number, data: Uint8Array }) => ipcRenderer.invoke('native:compressorSave', filePath, img),
  compressorLoad: (filePath: string) => ipcRenderer.invoke('native:compressorLoad', filePath),

  // 文件对话框相关函数
  openImageDialog: () => ipcRenderer.invoke('dialog:openImage'),
  savePngDialog: () => ipcRenderer.invoke('dialog:savePng'),
  savePpmDialog: () => ipcRenderer.invoke('dialog:savePpm'),
  openTripDialog: () => ipcRenderer.invoke('dialog:openTrip'),
  saveTripDialog: () => ipcRenderer.invoke('dialog:saveTrip'),
  saveImageDialog: () => ipcRenderer.invoke('dialog:saveImage'),
})
