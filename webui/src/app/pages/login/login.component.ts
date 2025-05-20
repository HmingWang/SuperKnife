import { Component, OnInit } from '@angular/core';
import {FormBuilder, FormGroup, ReactiveFormsModule, Validators} from '@angular/forms';
import { NzMessageService } from 'ng-zorro-antd/message';
import {NzFormControlComponent, NzFormDirective, NzFormItemComponent} from 'ng-zorro-antd/form';
import {NzTypographyComponent} from 'ng-zorro-antd/typography';
import {NzInputDirective, NzInputGroupComponent} from 'ng-zorro-antd/input';
import {NzCheckboxComponent} from 'ng-zorro-antd/checkbox';
import {NzButtonComponent} from 'ng-zorro-antd/button';
@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  imports: [
    NzFormItemComponent,
    NzTypographyComponent,
    ReactiveFormsModule,
    NzFormDirective,
    NzFormControlComponent,
    NzInputGroupComponent,
    NzCheckboxComponent,
    NzButtonComponent,
    NzInputDirective
  ],
  styleUrls: ['./login.component.scss'] // 修改为 .scss
})
export class LoginComponent implements OnInit {
  loginForm!: FormGroup;
  isLoading = false;

  constructor(private fb: FormBuilder, private message: NzMessageService) {}

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
      // 这里替换为你的登录逻辑
      console.log('Submit', this.loginForm.value);
      setTimeout(() => {
        this.isLoading = false;
        this.message.success('登录成功!');
      }, 1500);
    } else {
      Object.values(this.loginForm.controls).forEach(control => {
        if (control.invalid) {
          control.markAsDirty();
          control.updateValueAndValidity({ onlySelf: true });
        }
      });
    }
  }
}
