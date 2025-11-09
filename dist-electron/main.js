import { app, BrowserWindow, ipcMain } from "electron";
import { createRequire } from "node:module";
import { fileURLToPath } from "node:url";
import path from "node:path";
const require$1 = createRequire(import.meta.url);
const __dirname$1 = path.dirname(fileURLToPath(import.meta.url));
process.env.APP_ROOT = path.join(__dirname$1, "..");
const VITE_DEV_SERVER_URL = process.env["VITE_DEV_SERVER_URL"];
const MAIN_DIST = path.join(process.env.APP_ROOT, "dist-electron");
const RENDERER_DIST = path.join(process.env.APP_ROOT, "dist");
process.env.VITE_PUBLIC = VITE_DEV_SERVER_URL ? path.join(process.env.APP_ROOT, "public") : RENDERER_DIST;
let win;
function createWindow() {
  win = new BrowserWindow({
    icon: path.join(process.env.VITE_PUBLIC, "electron-vite.svg"),
    webPreferences: {
      preload: path.join(__dirname$1, "preload.mjs"),
      contextIsolation: true,
      nodeIntegration: false
    }
  });
  win.webContents.on("did-finish-load", () => {
    win == null ? void 0 : win.webContents.send("main-process-message", (/* @__PURE__ */ new Date()).toLocaleString());
  });
  if (VITE_DEV_SERVER_URL) {
    win.loadURL(VITE_DEV_SERVER_URL);
  } else {
    win.loadFile(path.join(RENDERER_DIST, "index.html"));
  }
}
app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
    win = null;
  }
});
app.on("activate", () => {
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow();
  }
});
app.whenReady().then(createWindow);
let native;
try {
  native = require$1(path.join(process.env.APP_ROOT, "native-addon"));
} catch (e) {
  console.error("Failed to load native addon. Please build it in native-addon.", e);
}
const toBuffer = (u8) => Buffer.from(u8.buffer, u8.byteOffset, u8.byteLength);
ipcMain.handle("native:loadPpm", async (_event, filePath) => {
  if (!native) throw new Error("native addon not loaded");
  return native.loadPpm(filePath);
});
ipcMain.handle("native:loadPng", async (_event, filePath) => {
  if (!native) throw new Error("native addon not loaded");
  return native.loadPng(filePath);
});
ipcMain.handle("native:savePpm", async (_event, filePath, img) => {
  if (!native) throw new Error("native addon not loaded");
  return native.savePpm(filePath, img.width, img.height, img.channels, toBuffer(img.data));
});
ipcMain.handle("native:savePng", async (_event, filePath, img) => {
  if (!native) throw new Error("native addon not loaded");
  return native.savePng(filePath, img.width, img.height, img.channels, toBuffer(img.data));
});
ipcMain.handle("native:toGray", async (_event, img) => {
  if (!native) throw new Error("native addon not loaded");
  return native.toGray(img.width, img.height, toBuffer(img.data));
});
ipcMain.handle("native:resize", async (_event, img, newW, newH) => {
  if (!native) throw new Error("native addon not loaded");
  return native.resize(img.width, img.height, img.channels, newW, newH, toBuffer(img.data));
});
ipcMain.handle("native:compressorSave", async (_event, filePath, img) => {
  if (!native) throw new Error("native addon not loaded");
  return native.compressorSave(filePath, img.width, img.height, img.channels, toBuffer(img.data));
});
ipcMain.handle("native:compressorLoad", async (_event, filePath) => {
  if (!native) throw new Error("native addon not loaded");
  return native.compressorLoad(filePath);
});
export {
  MAIN_DIST,
  RENDERER_DIST,
  VITE_DEV_SERVER_URL
};
