import path from 'path';
import { IpcMain } from 'electron';
import { BrowserWindow } from 'electron';
import bindings from 'bindings';

const addon = bindings('addon');

export function registerIpcMain(ipcMain: IpcMain) {
  ipcMain.handle('login', async (event, { username, password }) => {
    console.log('Received login request:', { username, password });
    try {
      const result = addon.verifyLogin(username, password);
      console.log('Login verification result:', result);
      return {
        success: result,
        username: result ? { username } : null
      };
    } catch (err) {
      console.error('登录验证失败:', err);
      return { success: false, error: err };
    }

  });
  ipcMain.on('minimize-window', async () => {
    BrowserWindow.getFocusedWindow()?.minimize();
  });
  ipcMain.on('maximize-window', async () => {
    BrowserWindow.getFocusedWindow()?.isMaximized() ? BrowserWindow.getFocusedWindow()?.unmaximize() : BrowserWindow.getFocusedWindow()?.maximize();
  });
  ipcMain.on('close-window', async () => {
    BrowserWindow.getFocusedWindow()?.close();
  });

}
