import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { Router } from '@angular/router';
import {ElectronService} from '../core/electron.service';


@Injectable({
  providedIn: 'root'
})
export class AuthService {
  private electronService: ElectronService=new ElectronService();

  constructor(private http: HttpClient, private router: Router) {}

  login(username: string, password: string) {
    this.electronService.invoke('login', {username, password}).then((response) => {
      console.log(response);
      this.storeAuthData(response);
    })
  }

  private storeAuthData(response: any): void {
    localStorage.setItem('auth_token', response.token);
    localStorage.setItem('current_user', JSON.stringify(response.user));
  }

  logout(): void {
    localStorage.removeItem('auth_token');
    localStorage.removeItem('current_user');
    this.router.navigate(['/login']);
  }

  isLoggedIn(): boolean {
    return !!localStorage.getItem('auth_token');
  }

  getToken(): string | null {
    return localStorage.getItem('auth_token');
  }
}
