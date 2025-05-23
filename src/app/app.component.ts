import { Component } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import {WindowControlsComponent} from './components/window-controls/window-controls.component';

@Component({
  selector: 'app-root',
  imports: [RouterOutlet, WindowControlsComponent],
  template: '<app-window-controls></app-window-controls><router-outlet/>',
  styles: []
})
export class AppComponent {
  title = 'webui';
}
