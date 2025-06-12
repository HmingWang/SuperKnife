import {Routes} from '@angular/router';
import {LoginComponent} from './pages/login/login.component';
import { DashboardComponent } from './pages/dashboard/dashboard.component';
import { authGuard } from './guards/auth.guard';

export const routes: Routes = [
  {path: '', redirectTo: 'login', pathMatch: 'full'},
  {path:'**', redirectTo: 'login', pathMatch: 'full'}, // Catch-all redirect to login
  {path: 'login', component: LoginComponent},
  {path:'dashboard',component:DashboardComponent,canActivate: [authGuard]},
];
