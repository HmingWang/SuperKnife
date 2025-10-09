import { Component, signal, computed, inject } from '@angular/core';
import { HttpClient, HttpHeaders, HttpParams } from '@angular/common/http';
import { NzMessageService } from 'ng-zorro-antd/message';
import { FormsModule } from '@angular/forms';
import { NzButtonModule } from 'ng-zorro-antd/button';
import { NzCardModule } from 'ng-zorro-antd/card';
import { NzInputModule } from 'ng-zorro-antd/input';
import { NzSelectModule } from 'ng-zorro-antd/select';
import { CommonModule } from '@angular/common';

@Component({
  selector: 'app-http-requester',
  standalone: true,
  templateUrl: './http-requester.component.html',
  styleUrls: ['./http-requester.component.scss'],
  imports: [
    CommonModule,
    FormsModule,
    NzCardModule,
    NzButtonModule,
    NzSelectModule,
    NzInputModule
  ] as const
})
export class HttpRequesterComponent {
  private http = inject(HttpClient);
  private message = inject(NzMessageService);

  method = signal<'GET' | 'POST'>('GET');
  url = signal('');
  params = signal<{ key: string; value: string }[]>([{ key: '', value: '' }]);
  headers = signal<{ key: string; value: string }[]>([{ key: '', value: '' }]);
  body = signal('');
  loading = signal(false);
  result = signal<any | null>(null);

  // 计算是否为POST
  isPost = computed(() => this.method() === 'POST');

  addParam() {
    this.params.update(list => [...list, { key: '', value: '' }]);
  }

  removeParam(i: number) {
    this.params.update(list => list.filter((_, idx) => idx !== i));
  }

  addHeader() {
    this.headers.update(list => [...list, { key: '', value: '' }]);
  }

  removeHeader(i: number) {
    this.headers.update(list => list.filter((_, idx) => idx !== i));
  }

  sendRequest() {
    if (!this.url().trim()) {
      this.message.error('请输入请求 URL');
      return;
    }

    this.loading.set(true);
    let httpParams = new HttpParams();
    let httpHeaders = new HttpHeaders();

    for (const { key, value } of this.params()) {
      if (key.trim()) httpParams = httpParams.append(key.trim(), value);
    }
    for (const { key, value } of this.headers()) {
      if (key.trim()) httpHeaders = httpHeaders.append(key.trim(), value);
    }

    const options = { params: httpParams, headers: httpHeaders };

    if (this.method() === 'GET') {
      this.http.get(this.url(), options).subscribe({
        next: res => this.handleResult(res),
        error: err => this.handleResult(err, true)
      });
    } else {
      let data: any = {};
      try {
        data = this.body() ? JSON.parse(this.body()) : {};
      } catch {
        this.message.error('请求体 JSON 格式不正确');
        this.loading.set(false);
        return;
      }

      this.http.post(this.url(), data, options).subscribe({
        next: res => this.handleResult(res),
        error: err => this.handleResult(err, true)
      });
    }
  }

  private handleResult(res: any, isError = false) {
    this.result.set(res);
    this.loading.set(false);
    if (isError) this.message.error('请求失败');
  }
}
