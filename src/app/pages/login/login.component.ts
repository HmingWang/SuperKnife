import { Component, OnInit } from '@angular/core';
import { FormBuilder, FormGroup, ReactiveFormsModule, Validators } from '@angular/forms';
import { NzMessageService } from 'ng-zorro-antd/message';
import { NzFormControlComponent, NzFormDirective, NzFormItemComponent } from 'ng-zorro-antd/form';
import { NzInputDirective, NzInputGroupComponent } from 'ng-zorro-antd/input';
import { NzButtonComponent } from 'ng-zorro-antd/button';
import { AuthService } from '../../services/auth.service';
import { Router } from '@angular/router';
import { ElectronService } from '../../core/electron.service';
import { StatusService } from '../../services/status.service';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  imports: [
    NzFormDirective,
    ReactiveFormsModule,
    NzFormItemComponent,
    NzInputGroupComponent,
    NzFormControlComponent,
    NzInputDirective,
    NzButtonComponent,

  ],
  styleUrls: ['./login.component.scss']
})
export class LoginComponent implements OnInit {

  loginForm!: FormGroup;
  isLoading = false;
  electronService: ElectronService = new ElectronService();
  showMenuButton = false;

  constructor(private fb: FormBuilder,
    private message: NzMessageService,
    private authService: AuthService,
    private router: Router,
    private statusService: StatusService) {
    this.loginForm = this.fb.group({
      username: [null, [Validators.required]],
      password: [null, [Validators.required]],
      remember: [true]
    });
  }

  ngOnInit(): void {
    this.loginForm = this.fb.group({
      username: [null, [Validators.required]],
      password: [null, [Validators.required]],
      remember: [true]
    });
    this.electronService.resizeWindow(320, 450); // Resize window after login
    
    this.statusService.showMenuButton.asObservable().subscribe((visible: boolean) => {
      if (visible) {
        this.showMenuButton = visible;
      }
    });    
  }

  submitForm() {
    if (this.loginForm.valid) {
      this.isLoading = true;
      console.log('Login form submitted:', this.loginForm.value);
      const { username, password } = this.loginForm.value;

      this.authService.login(username, password).then((result) => {
        this.isLoading = false;
        console.log('Login result:', result);
        if (result) {
          this.message.success('登录成功');
          this.router.navigate(['/dashboard']);
          this.electronService.resizeWindow(800, 600); // Resize window after login
        } else {
          this.message.error('登录失败，请检查用户名和密码');
        }
      }).catch((error) => {
        this.isLoading = false;
        console.error('Login error:', error);
        this.message.error('登录失败，请稍后再试');
      });  
    }
  }
}
