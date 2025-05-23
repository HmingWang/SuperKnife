import {Component} from '@angular/core';

import {ElectronService} from '../../core/electron.service';
import {NzIconDirective} from 'ng-zorro-antd/icon';

@Component({
  selector: 'app-window-controls',
  templateUrl: './window-controls.component.html',
  styleUrls: ['./window-controls.component.scss'],
  imports: [
    NzIconDirective

  ]

})
export class WindowControlsComponent {

  private electronService: ElectronService=new ElectronService();

  minimize() {
    this.electronService.send('minimize-window',{});
  }

  maximize() {
    this.electronService.send('maximize-window',{});
  }

  close() {
    this.electronService.send('close-window',{});
  }
}
