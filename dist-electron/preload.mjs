"use strict";
const electron = require("electron");
electron.contextBridge.exposeInMainWorld("ipcRenderer", {
  on(...args) {
    const [channel, listener] = args;
    return electron.ipcRenderer.on(channel, (event, ...args2) => listener(event, ...args2));
  },
  off(...args) {
    const [channel, ...omit] = args;
    return electron.ipcRenderer.off(channel, ...omit);
  },
  send(...args) {
    const [channel, ...omit] = args;
    return electron.ipcRenderer.send(channel, ...omit);
  },
  invoke(...args) {
    const [channel, ...omit] = args;
    return electron.ipcRenderer.invoke(channel, ...omit);
  }
  // You can expose other APTs you need here.
  // ...
});
electron.contextBridge.exposeInMainWorld("native", {
  loadPpm: (filePath) => electron.ipcRenderer.invoke("native:loadPpm", filePath),
  loadPng: (filePath) => electron.ipcRenderer.invoke("native:loadPng", filePath),
  savePpm: (filePath, img) => electron.ipcRenderer.invoke("native:savePpm", filePath, img),
  savePng: (filePath, img) => electron.ipcRenderer.invoke("native:savePng", filePath, img),
  toGray: (img) => electron.ipcRenderer.invoke("native:toGray", img),
  resize: (img, newW, newH) => electron.ipcRenderer.invoke("native:resize", img, newW, newH),
  compressorSave: (filePath, img) => electron.ipcRenderer.invoke("native:compressorSave", filePath, img),
  compressorLoad: (filePath) => electron.ipcRenderer.invoke("native:compressorLoad", filePath),
  // File dialogs
  openImageDialog: () => electron.ipcRenderer.invoke("dialog:openImage"),
  savePngDialog: () => electron.ipcRenderer.invoke("dialog:savePng"),
  savePpmDialog: () => electron.ipcRenderer.invoke("dialog:savePpm"),
  openTripDialog: () => electron.ipcRenderer.invoke("dialog:openTrip"),
  saveTripDialog: () => electron.ipcRenderer.invoke("dialog:saveTrip"),
  saveImageDialog: () => electron.ipcRenderer.invoke("dialog:saveImage")
});
