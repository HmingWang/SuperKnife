import { app, BrowserWindow, ipcMain, crashReporter } from 'electron';
import path from 'path';
import fs from 'fs';
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
  // if (isDev) {
  //   console.log("env:development");
  //   mainWindow.webContents.openDevTools();
  //   mainWindow.loadURL("http://localhost:4200/index.html");
  //   mainWindow.resizable = true;
  //   mainWindow.setSize(320+400,450);
  // } else {
  //   console.log("env:production");
  //   mainWindow.loadFile(path.join(path.resolve(), './dist/renderer/browser/index.html'));
  // }

  if (isDev) {

    console.log("env:development");
    mainWindow.webContents.openDevTools();
    mainWindow.loadURL("http://localhost:4200/index.html");
    mainWindow.resizable = true;
    mainWindow.setSize(320, 450);
  } else {
    console.log("env:production");

    // Path when running electron executable
    let pathIndex = './index.html';

    if (fs.existsSync(path.join(__dirname, './dist/renderer/browser/index.html'))) {
      // Path when running electron in local folder
      pathIndex = './dist/renderer/browser/index.html';
    }

    const fullPath = path.join(__dirname, pathIndex);
    const url = `file://${path.resolve(fullPath).replace(/\\/g, '/')}`;
    mainWindow.loadURL(url);
  }


}

// Error Handling
process.on('uncaughtException', (error) => {
  console.error("Unexpected error: ", error);
});

app.whenReady().then(() => {
  isDev = process.env["BUILD_TYPE"] === "dev";

  console.log(app.getPath('crashDumps'))
  registerIpcMain(ipcMain);

  createWindow();



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
