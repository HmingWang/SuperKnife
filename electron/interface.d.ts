// export interface IElectronAPI {
//   minimizeWindow():void,
//   maximizeWindow():void,
//   closeWindow():void,
//   login(username:string,password:string):Promise<LoginResponse>,
// }

declare global {
  interface Window {
    electronAPI: {
      send: (channel: string, data: any) => void;
      on: (channel: string, func: (...args: any[]) => void) => void;
      invoke: (channel: string, data: any) => Promise<any>;
    };
  }
}

//   interface LoginResponse {
//     token: string;
//     user: {
//       id: number;
//       username: string;
//       // 其他用户字段...
//     };
//   }
// }
