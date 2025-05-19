import {app, BrowserWindow, screen} from 'electron';
import * as path from 'path';
import * as fs from 'fs';

function createWindow () {
  const win = new BrowserWindow({
    width: 800,
    height: 600
  })

  console.log(__dirname)
  win.loadFile(path.join(__dirname,'/dist/webui/brower/index.html'))
}

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