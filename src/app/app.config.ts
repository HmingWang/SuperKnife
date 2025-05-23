import { ApplicationConfig, provideZoneChangeDetection, importProvidersFrom } from '@angular/core';
import { provideRouter } from '@angular/router';

import { routes } from './app.routes';
import { en_US, provideNzI18n } from 'ng-zorro-antd/i18n';
import { registerLocaleData } from '@angular/common';
import en from '@angular/common/locales/en';
import { provideAnimationsAsync } from '@angular/platform-browser/animations/async';
import {provideHttpClient, withInterceptors} from '@angular/common/http';
import { IconDefinition } from '@ant-design/icons-angular';

import {provideNzIcons} from 'ng-zorro-antd/icon';
// import { UserOutline, LockOutline,CloseOutline,BorderOutline,MinusOutline,ExpandOutline } from '@ant-design/icons-angular/icons';
import * as AllIcons from '@ant-design/icons-angular/icons';

// 按需导入 NG-ZORRO 模块
import { NzFormModule } from 'ng-zorro-antd/form';
import { NzInputModule } from 'ng-zorro-antd/input';
import { NzButtonModule } from 'ng-zorro-antd/button';
import { NzCheckboxModule } from 'ng-zorro-antd/checkbox';
import {authInterceptor} from './interceptors/auth.interceptor';

registerLocaleData(en);

// const icons: IconDefinition[] = [ UserOutline, LockOutline,CloseOutline,BorderOutline,MinusOutline,ExpandOutline];


const antDesignIcons = AllIcons as Record<string, IconDefinition>;
const icons: IconDefinition[] = Object.keys(antDesignIcons).map(key => antDesignIcons[key])


export const appConfig: ApplicationConfig = {
  providers: [
    provideNzIcons(icons),
    provideZoneChangeDetection({ eventCoalescing: true }),
    provideRouter(routes),
    provideNzI18n(en_US),
    importProvidersFrom(
      NzFormModule,
      NzInputModule,
      NzButtonModule,
      NzCheckboxModule,
    ),
    provideAnimationsAsync(),
    provideHttpClient(
      withInterceptors([authInterceptor])
    )],
};
