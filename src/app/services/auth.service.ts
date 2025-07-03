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

  async login(username: string, password: string): Promise<boolean> {
    return this.electronService.invoke('login', {username, password}).then((result) => {
      console.log(result);
      if (!result.success) {
        return false; // Login failed
      }
      
      this.storeAuthData(result);
      console.log('Login successful:', result);
      return true; // Login successful
    })
    .catch((error) => {
      console.error('Login error:', error);
      return false; // Handle error
    });
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
