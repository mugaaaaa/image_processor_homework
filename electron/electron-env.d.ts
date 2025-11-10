/// <reference types="vite-plugin-electron/electron-env" />

declare namespace NodeJS {
  interface ProcessEnv {
    /**
     * The built directory structure
     *
     * ```tree
     * ├─┬─┬ dist
     * │ │ └── index.html
     * │ │
     * │ ├─┬ dist-electron
     * │ │ ├── main.js
     * │ │ └── preload.js
     * │
     * ```
     */
    APP_ROOT: string
    /** /dist/ or /public/ */
    VITE_PUBLIC: string
  }
}

// Used in Renderer process, expose in `preload.ts`
interface Window {
  ipcRenderer: import('electron').IpcRenderer
  native: {
    loadPpm: (filePath: string) => Promise<{ width: number; height: number; channels: number; data: Uint8Array }>
    loadPng: (filePath: string) => Promise<{ width: number; height: number; channels: number; data: Uint8Array }>
    savePpm: (filePath: string, img: { width: number; height: number; channels: number; data: Uint8Array }) => Promise<boolean>
    savePng: (filePath: string, img: { width: number; height: number; channels: number; data: Uint8Array }) => Promise<boolean>
    toGray: (img: { width: number; height: number; data: Uint8Array }) => Promise<{ width: number; height: number; channels: number; data: Uint8Array }>
    resize: (img: { width: number; height: number; channels: number; data: Uint8Array }, newW: number, newH: number) => Promise<{ width: number; height: number; channels: number; data: Uint8Array }>
    compressorSave: (filePath: string, img: { width: number; height: number; channels: number; data: Uint8Array }) => Promise<boolean>
    compressorLoad: (filePath: string) => Promise<{ width: number; height: number; channels: number; data: Uint8Array }>

    // File dialog APIs
    openImageDialog: () => Promise<string | null>
    savePngDialog: () => Promise<string | null>
    savePpmDialog: () => Promise<string | null>
    openTripDialog: () => Promise<string | null>
    saveTripDialog: () => Promise<string | null>
    saveImageDialog: () => Promise<string | null>
  }
}
