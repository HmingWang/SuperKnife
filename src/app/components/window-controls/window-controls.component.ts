import { Component, Input, Output } from '@angular/core';

import { ElectronService } from '../../core/electron.service';
import { NzIconDirective } from 'ng-zorro-antd/icon';
import { WindowControlService } from '../../services/window-control.service';

@Component({
  selector: 'app-window-controls',
  templateUrl: './window-controls.component.html',
  styleUrls: ['./window-controls.component.scss'],
  imports: [
    NzIconDirective,
]

})
export class WindowControlsComponent {
  changeCollapse() {
    this.isCollapsed = !this.isCollapsed;
    this.windowControlService.setOptions({
      collapsible: this.isCollapsed
    })
  }

  options: any;

  isCollapsed: boolean = false;

  constructor(private electronService: ElectronService,
    private windowControlService: WindowControlService
  ) { }

  ngOnInit(): void {
    this.options = this.windowControlService.options;
  }

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
