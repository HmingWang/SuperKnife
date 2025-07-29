import { Component, Input, Output } from '@angular/core';

import { ElectronService } from '../../core/electron.service';
import { NzIconDirective } from 'ng-zorro-antd/icon';
import { StatusService } from '../../services/status.service';

@Component({
  selector: 'app-window-controls',
  templateUrl: './window-controls.component.html',
  styleUrls: ['./window-controls.component.scss'],
  imports: [
    NzIconDirective

  ]

})
export class WindowControlsComponent {


  @Input() title!: string;

  @Output() isCollapsed: boolean = false;
  @Input() showMenuButton: boolean = false;

  constructor(private electronService: ElectronService,
    private statusService: StatusService
  ) { }

  minimize() {
    this.electronService.send('minimize-window', {});
  }

  maximize() {
    this.electronService.send('maximize-window', {});
  }

  close() {
    this.electronService.send('close-window', {});
  }
}
