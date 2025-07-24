import { Routes } from '@angular/router';
import { LoginComponent } from './pages/login/login.component';
import { DashboardComponent } from './pages/dashboard/dashboard.component';
import { authGuard } from './guards/auth.guard';
import { FirewallComponent } from './pages/firewall/firewall.component';

export const routes: Routes = [
  { path: '', redirectTo: 'login', pathMatch: 'full' },
  { path: 'login', component: LoginComponent },
  { path: 'dashboard', component: DashboardComponent,canActivate: [authGuard], 
    // Add child routes for the dashboard here
    children: [
      { path: 'firewall', component: FirewallComponent,outlet:'inner-content' },
    ]
  },
  { path: '**', redirectTo: 'login', pathMatch: 'full' }, // Catch-all redirect to login

];
