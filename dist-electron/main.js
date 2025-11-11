import { app, BrowserWindow, ipcMain, dialog, Menu } from "electron";
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
    width: 1024,
    height: 612,
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
  try {
    Menu.setApplicationMenu(null);
    win.setMenuBarVisibility(false);
  } catch (e) {
    console.error("Failed to remove application menu", e);
  }
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
ipcMain.handle("dialog:openImage", async () => {
  const result = await dialog.showOpenDialog(win, {
    properties: ["openFile"],
    filters: [
      { name: "Images", extensions: ["png", "ppm"] },
      { name: "All Files", extensions: ["*"] }
    ]
  });
  return result.canceled ? null : result.filePaths[0];
});
ipcMain.handle("dialog:savePng", async () => {
  const result = await dialog.showSaveDialog(win, {
    filters: [{ name: "PNG Image", extensions: ["png"] }],
    defaultPath: "image.png"
  });
  return result.canceled ? null : result.filePath;
});
ipcMain.handle("dialog:savePpm", async () => {
  const result = await dialog.showSaveDialog(win, {
    filters: [{ name: "PPM Image", extensions: ["ppm"] }],
    defaultPath: "image.ppm"
  });
  return result.canceled ? null : result.filePath;
});
ipcMain.handle("dialog:openTrip", async () => {
  const result = await dialog.showOpenDialog(win, {
    properties: ["openFile"],
    filters: [
      { name: "Trip Files", extensions: ["trip"] },
      { name: "All Files", extensions: ["*"] }
    ]
  });
  return result.canceled ? null : result.filePaths[0];
});
ipcMain.handle("dialog:saveTrip", async () => {
  const result = await dialog.showSaveDialog(win, {
    filters: [{ name: "Trip Files", extensions: ["trip"] }],
    defaultPath: "image.trip"
  });
  return result.canceled ? null : result.filePath;
});
export {
  MAIN_DIST,
  RENDERER_DIST,
  VITE_DEV_SERVER_URL
};
