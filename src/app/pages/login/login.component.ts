import { Component, OnInit } from '@angular/core';
import { FormBuilder, FormGroup, ReactiveFormsModule, Validators } from '@angular/forms';
import { NzMessageService } from 'ng-zorro-antd/message';
import { NzFormControlComponent, NzFormDirective, NzFormItemComponent } from 'ng-zorro-antd/form';
import { NzInputDirective, NzInputGroupComponent } from 'ng-zorro-antd/input';
import { NzButtonComponent } from 'ng-zorro-antd/button';
import { NzCheckboxComponent } from 'ng-zorro-antd/checkbox';
import { AuthService } from '../../services/auth.service';
import { Router } from '@angular/router';
import { ElectronService } from '../../core/electron.service';
import { WindowControlService } from '../../services/window-control.service';
import { NgxParticlesModule } from "@tsparticles/angular";
import { Engine, type ISourceOptions } from "@tsparticles/engine";
import { loadSlim } from "@tsparticles/slim";

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
    NzCheckboxComponent,
    NgxParticlesModule
  ],
  styleUrls: ['./login.component.scss']
})
export class LoginComponent implements OnInit {

  loginForm!: FormGroup;
  isLoading = false;
  electronService: ElectronService = new ElectronService();
  showMenuButton = false;

  particlesOptions: ISourceOptions = {
    background: {
      color: {
        value: "#0d47a1",
      },
    },
    fullScreen: {
      enable: true,
      zIndex: -1
    },
    fpsLimit: 120,
    interactivity: {
      events: {
        onClick: {
          enable: true,
          mode: "push",
        },
        onHover: {
          enable: true,
          mode: "repulse",
        },
        resize: {
          enable: true,
          delay: 0.5
        },
      },
      modes: {
        push: {
          quantity: 4,
        },
        repulse: {
          distance: 200,
          duration: 0.4,
        },
      },
    },
    particles: {
      color: {
        value: "#ffffff",
      },
      links: {
        color: "#ffffff",
        distance: 150,
        enable: true,
        opacity: 0.5,
        width: 1,
      },
      move: {
        direction: "none",
        enable: true,
        outModes: {
          default: "bounce",
        },
        random: false,
        speed: 6,
        straight: false,
      },
      number: {
        value: 80,
        density: {
          enable: true
        }
      },
      opacity: {
        value: 0.5,
      },
      shape: {
        type: "circle",
      },
      size: {
        value: { min: 1, max: 5 },
      },
    },
    detectRetina: true,
  };

  constructor(private fb: FormBuilder,
    private message: NzMessageService,
    private authService: AuthService,
    private router: Router,private windowControlService: WindowControlService) {
  }

  async particlesInit(engine: Engine): Promise<void> {
    await loadSlim(engine);
  }

  ngOnInit(): void {
    const rememberedUsername = localStorage.getItem('remembered_username');
    this.loginForm = this.fb.group({
      username: [rememberedUsername, [Validators.required]],
      password: [null, [Validators.required]],
      remember: [!!rememberedUsername]
    });

    this.electronService.resizeWindow(400, 600); // 调整登录窗口大小
    this.windowControlService.options().transparent = true;


  }

  submitForm() {
    if (this.loginForm.valid) {
      this.isLoading = true;
      console.log('Login form submitted:', this.loginForm.value);
      const { username, password, remember } = this.loginForm.value;

      this.authService.login(username, password).then((result) => {
        this.isLoading = false;
        console.log('Login result:', result);
        if (result) {
          if (remember) {
            localStorage.setItem('remembered_username', username);
          } else {
            localStorage.removeItem('remembered_username');
          }
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

  forgotPassword() {
    this.message.info('请联系系统管理员重置密码');
  }
}
