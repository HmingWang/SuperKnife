import path from 'path';
import { IpcMain } from 'electron';
import{ BrowserWindow } from 'electron';

const bindings = require('bindings')
const addon = bindings('addon');

export function registerIpcMain(ipcMain: IpcMain) {
    // ipcMain.on('login', async (event, {username,password}) => {
    //    try {
    //   const result = addon.verifyLogin(username, password);
    //   return {
    //     success: result,
    //     user: result ? { username } : null
    //   };
    // } catch (err) {
    //   console.error('登录验证失败:', err);
    //   return { success: false, error: err };
    // }

    // });


  }
