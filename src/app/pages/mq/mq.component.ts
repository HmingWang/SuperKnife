import { Component } from '@angular/core';
import { FormsModule } from '@angular/forms';

import { NzSelectModule } from 'ng-zorro-antd/select';
import { NzButtonModule } from 'ng-zorro-antd/button';
import { NzModalModule } from 'ng-zorro-antd/modal';
import { NzInputModule } from 'ng-zorro-antd/input';

@Component({
  selector: 'app-mq',
  imports: [NzSelectModule, FormsModule, NzButtonModule, NzModalModule, NzInputModule],
  templateUrl: './mq.component.html',
  styleUrl: './mq.component.scss'
})
export class MqComponent {
  selectedValue = null;

  showAddModal = false;

  config={
    name: '',
    host: '',
    port: 1414,
    channel: '',
    qmgr: '',
    queue: '',
    username: '',
    password: ''
  }

  add_options(): void {
    this.showAddModal = true;
  }
}
