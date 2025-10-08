import { Component } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { WindowControlsComponent } from './components/window-controls/window-controls.component';
import { WindowControlService } from './services/window-control.service';

@Component({
  selector: 'app-root',
  imports: [RouterOutlet, WindowControlsComponent],
  template: `
    <app-window-controls/>
    <div class="content">
      <router-outlet/>
    </div>
    `,
  styles: [
    `
      .content{
        margin-top: 28px;
      }
    `
  ]
})
export class AppComponent {
  constructor(private windowControlService: WindowControlService) {
    this.windowControlService.options().title = "Super Knife";
  }
}
