import {app, BrowserWindow, ipcMain} from 'electron';
import path from 'path';

let isDev = !app.isPackaged;
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
  if (!isDev) {
    console.log("env:production");
    mainWindow.loadFile(path.join(path.resolve(), './dist/renderer/browser/index.html'));
  } else {
    console.log("env:development");
    // mainWindow.webContents.openDevTools();
    mainWindow.loadURL("http://localhost:4200");
  }

  ipcMain.on('login', async (event, credentials) => {
    // 这里替换为你的实际登录逻辑
    // 可以是数据库验证、文件系统验证等
    const {username, password} = credentials;

    if (username === 'admin' && password === 'password') {
      return {
        success: true,
        token: 'fake-jwt-token',
        user: {id: 1, username: 'admin'}
      };
    } else {
      throw new Error('Invalid credentials');
    }
  });
  ipcMain.on('minimize-window', () => {
    mainWindow?.minimize();
  });
  ipcMain.on('maximize-window', () => {
    mainWindow?.isMaximized()?mainWindow?.unmaximize():mainWindow?.maximize();
  });
  ipcMain.on('close-window', () => {
    mainWindow?.close();
  });
  ipcMain.on('environment', (event, prod: boolean) => {
    isDev = !prod;
  });
}

// Error Handling
process.on('uncaughtException', (error) => {
  console.error("Unexpected error: ", error);
});

app.whenReady().then(() => {
  createWindow()


  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow()
    }
  })
})

app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    app.quit()
  }
})
