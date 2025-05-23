import { app, BrowserWindow, ipcMain, IpcMainEvent } from 'electron';
import path from 'path';


function createWindow () {
    const win = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            preload: path.join(__dirname, './dist/electron/preload.js'),
            contextIsolation: true,
        },
        frame: false,
        titleBarStyle: 'hidden',
    });
    win.loadFile(path.join(__dirname, './dist/browser/index.html'));
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
