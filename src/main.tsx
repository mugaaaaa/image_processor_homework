/* eslint-disable @typescript-eslint/no-explicit-any */
/**
 * @file main.ts
 * @author Runhui Mo (github.com/mugaaaaa)
 * @brief Electron 主渲染进程入口文件
 * 
 * @version 0.1
 * @date 2025-11-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

import React from 'react'
import ReactDOM from 'react-dom/client'
import App from './App.tsx'
import './index.css'

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <App />
  </React.StrictMode>,
)

// Use contextBridge (guard in non-Electron preview)
;(window as any).ipcRenderer?.on?.('main-process-message', (_event: any, message: string) => {
  console.log(message)
})
