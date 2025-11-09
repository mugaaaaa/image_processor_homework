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

// Expose high-level API for Renderer
contextBridge.exposeInMainWorld('native', {
  loadPpm: (filePath: string) => ipcRenderer.invoke('native:loadPpm', filePath),
  loadPng: (filePath: string) => ipcRenderer.invoke('native:loadPng', filePath),
  savePpm: (filePath: string, img: { width: number, height: number, channels: number, data: Uint8Array }) => ipcRenderer.invoke('native:savePpm', filePath, img),
  savePng: (filePath: string, img: { width: number, height: number, channels: number, data: Uint8Array }) => ipcRenderer.invoke('native:savePng', filePath, img),
  toGray: (img: { width: number, height: number, data: Uint8Array }) => ipcRenderer.invoke('native:toGray', img),
  resize: (img: { width: number, height: number, channels: number, data: Uint8Array }, newW: number, newH: number) => ipcRenderer.invoke('native:resize', img, newW, newH),
  compressorSave: (filePath: string, img: { width: number, height: number, channels: number, data: Uint8Array }) => ipcRenderer.invoke('native:compressorSave', filePath, img),
  compressorLoad: (filePath: string) => ipcRenderer.invoke('native:compressorLoad', filePath),
})
