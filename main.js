const { app, BrowserWindow,ipcMain } = require('electron');
const path = require('path');


function createWindow () {
    const win = new BrowserWindow({
        width: 800,
        height: 600,
        webPreferences: {
            preload: path.join(__dirname, 'preload.js'),
            contextIsolation: true,
            enableRemoteModule: false,
        }
    });
    win.loadFile(path.join(__dirname, './dist/webui/browser/index.html'));
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