import { Routes } from '@angular/router';
import { LoginComponent } from './pages/login/login.component';
import { DashboardComponent } from './pages/dashboard/dashboard.component';
import { authGuard } from './guards/auth.guard';
import { FirewallComponent } from './pages/firewall/firewall.component';
import { Base64Component } from './pages/base64/base64.component';
import { WebgpuComponent } from './pages/webgpu/webgpu.component';
import { HttpRequesterComponent } from './pages/http-requester/http-requester.component';
import { MqComponent } from './pages/mq/mq.component';

export const routes: Routes = [
  { path: '', redirectTo: 'login', pathMatch: 'full' },
  { path: 'login', component: LoginComponent },
  {
    path: 'dashboard', component: DashboardComponent,
    // Add child routes for the dashboard here
    children: [
      { path: '', redirectTo: 'base64', pathMatch: 'full' }, // Default child route
      { path: 'firewall', component: FirewallComponent },
      { path: 'base64', component: Base64Component },
      { path: 'webgpu', component: WebgpuComponent },
      { path: 'http', component: HttpRequesterComponent },
      { path: 'mq', component: MqComponent },

      { path: '**', redirectTo: 'base64', pathMatch: 'full' }// Catch-all for dashboard
    ]
  },
  { path: '**', redirectTo: 'login', pathMatch: 'full' }, // Catch-all redirect to login

];
