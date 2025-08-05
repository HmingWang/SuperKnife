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
  changeCollapse() {
    this.statusService.isCollapsed.next(!this.statusService.isCollapsed.value);
    this.isCollapsed=this.statusService.isCollapsed.value;
  }


  @Input() title!: string;

  isCollapsed: boolean = false;
  @Input() showMenuButton: boolean = false;

  constructor(private electronService: ElectronService,
    private statusService: StatusService
  ) { }


  ngOnInit(): void {
    this.statusService.showMenuButton.asObservable().subscribe((visible: boolean) => {
      this.showMenuButton = visible;
    });
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
