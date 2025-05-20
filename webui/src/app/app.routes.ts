import {Routes} from '@angular/router';
import {LoginComponent} from './pages/login/login.component';
import {authGuard} from './routers/auth.guard';

export const routes: Routes = [
  {path: '', redirectTo: 'login', pathMatch: 'full'},
  {path: 'login', component: LoginComponent},
  // {path: 'main', canActivate: [authGuard]}
];
