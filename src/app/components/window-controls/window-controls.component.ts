import { Component } from '@angular/core';
import {NzButtonComponent} from 'ng-zorro-antd/button';
import {NzIconDirective} from 'ng-zorro-antd/icon';

declare const window: any;

@Component({
  selector: 'app-window-controls',
  templateUrl: './window-controls.component.html',
  styleUrls: ['./window-controls.component.scss'],
  imports: [
    NzButtonComponent,
    NzIconDirective
  ]

})
export class WindowControlsComponent {
  minimize() {
    if (window.electron) {
      window.electron.ipcRenderer.invoke('window-minimize');
    }
  }

  maximize() {
    if (window.electron) {
      window.electron.ipcRenderer.invoke('window-maximize');
    }
  }

  close() {
    if (window.electron) {
      window.electron.ipcRenderer.invoke('window-close');
    }
  }
}
