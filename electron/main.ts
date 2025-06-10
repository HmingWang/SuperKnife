import { app, BrowserWindow, ipcMain, crashReporter } from 'electron';
import path from 'path';
import { registerIpcMain } from './bridge';

let isDev: boolean = false;
let mainWindow: BrowserWindow | null = null;

function createWindow() {


  mainWindow = new BrowserWindow({
    width: 320,
    height: 450,
    webPreferences: {
      preload: path.join(path.resolve(), './dist/electron/preload.js'),
    },
    frame: false, //隐藏边框
    // transparent: true, // 可选：透明背景
    // titleBarStyle: 'hidden'//macos :hiddenInset/hidden仅隐藏标题栏，但显示控制按钮。
    resizable: false,

  });
  if (isDev) {
    console.log("env:development");
    mainWindow.webContents.openDevTools();
    mainWindow.loadURL("http://localhost:4200");
    mainWindow.resizable = true;
  } else {
    console.log("env:production");
    mainWindow.loadFile(path.join(path.resolve(), './dist/renderer/browser/index.html'));
  }


}

// Error Handling
process.on('uncaughtException', (error) => {
  console.error("Unexpected error: ", error);
});

app.whenReady().then(() => {
  isDev = process.env["BUILD_TYPE"] === "dev";

  console.log(app.getPath('crashDumps'))

  createWindow();

  registerIpcMain(ipcMain);


})

app.on('activate', () => {
  if (BrowserWindow.getAllWindows().length === 0) {
    createWindow()
  }
})
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})
