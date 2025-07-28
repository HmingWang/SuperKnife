import { IpcMain } from 'electron';
import { BrowserWindow } from 'electron';
import { AddonService } from './addon';

const addonService = new AddonService();

export function registerIpcMain(ipcMain: IpcMain) {
  ipcMain.on('minimize-window', async () => {
    BrowserWindow.getFocusedWindow()?.minimize();
  });
  ipcMain.on('maximize-window', async () => {
    BrowserWindow.getFocusedWindow()?.isMaximized() ? BrowserWindow.getFocusedWindow()?.unmaximize() : BrowserWindow.getFocusedWindow()?.maximize();
  });
  ipcMain.on('close-window', async () => {
    BrowserWindow.getFocusedWindow()?.close();
  });

  ipcMain.on('resize-window', async (event, { width, height }) => {
    const window = BrowserWindow.getFocusedWindow();
    if (window) {
      window.setSize(width, height);
      console.log(`Window resized to ${width}x${height}`);
    } else {
      console.error('No focused window found to resize.');
    }
  });
  ipcMain.handle('login', async (event, { username, password }) => {
    const result = addonService.verifyLogin(username, password);
    return result;
  });
  ipcMain.handle('encodeBase64', async (event, { data, wrap }) => {
    const result = addonService.encodeBase64(data, wrap);
    return result;
  });

  ipcMain.handle('decodeBase64', async (event, { data }) => {
    const result = addonService.decodeBase64(data);
    return result;
  });
}
