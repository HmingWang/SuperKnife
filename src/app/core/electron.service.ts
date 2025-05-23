import { Injectable } from '@angular/core';
import{environment} from '../../environments/environment';

declare global {
  interface Window {
    electronAPI: {
      send: (channel: string, data: any) => void;
      on: (channel: string, func: (...args: any[]) => void) => void;
      invoke: (channel: string, data: any) => Promise<any>;
    };
  }
}

@Injectable({
  providedIn: 'root'
})


export class ElectronService {

  private env=environment;


  constructor() {
    this.initEnviroment();
  }

  initEnviroment(): void {
    window.electronAPI.send("environment",this.env.production);
  }

  send(channel: string, data: any) {
    window.electronAPI.send(channel, data);
  }

  on(channel: string, func: (...args: any[]) => void) {
    window.electronAPI.on(channel, func);
  }

  invoke(channel: string, data: any): Promise<any> {
    return window.electronAPI.invoke(channel, data);
  }
}
