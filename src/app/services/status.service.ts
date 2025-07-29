import { Injectable } from '@angular/core';

@Injectable({
  providedIn: 'root'
})
export class StatusService {

  
  isLogin: boolean = false;
  menuButtonVisible: boolean = false;

  constructor() { }

  setLoginStatus(status: boolean) {
    this.isLogin = status;
  }
  setMenuButtonVisibility(visible: boolean) {
    this.menuButtonVisible = visible;
  }
  getLoginStatus(): boolean {
    return this.isLogin;
  }
  getMenuButtonVisibility(): boolean {
    return this.menuButtonVisible;
  }

}
