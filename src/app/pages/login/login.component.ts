import {Component, OnInit} from '@angular/core';
import {FormBuilder, FormGroup, ReactiveFormsModule, Validators} from '@angular/forms';
import {NzMessageService} from 'ng-zorro-antd/message';
import {NzFormControlComponent, NzFormDirective, NzFormItemComponent} from 'ng-zorro-antd/form';
import {NzInputDirective, NzInputGroupComponent} from 'ng-zorro-antd/input';
import {NzCheckboxComponent} from 'ng-zorro-antd/checkbox';
import {NzButtonComponent} from 'ng-zorro-antd/button';
import {AuthService} from '../../services/auth.service';
import {ActivatedRoute, Router} from '@angular/router';
import {ElectronService} from '../../core/electron.service';

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
    NzCheckboxComponent,
    NzButtonComponent,

  ],
  styleUrls: ['./login.component.scss']
})
export class LoginComponent implements OnInit {
  loginForm!: FormGroup;
  isLoading = false;
  electronService: ElectronService=new ElectronService();

  constructor(private fb: FormBuilder,
              private message: NzMessageService,
              private authService: AuthService,
              private router: Router,
              private route: ActivatedRoute) {
    this.loginForm = this.fb.group({
      userName: [null, [Validators.required]],
      password: [null, [Validators.required]],
      remember: [true]
    });
  }

  ngOnInit(): void {
    this.loginForm = this.fb.group({
      userName: [null, [Validators.required]],
      password: [null, [Validators.required]],
      remember: [true]
    });
  }

  submitForm(): void {
    if (this.loginForm.valid) {
      this.isLoading = true;
      const {username, password} = this.loginForm.value;

      this.electronService.send('minimize-window',{});
    }
  }
}
