import { Component } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { NzButtonModule } from 'ng-zorro-antd/button';
import { NzCheckboxModule } from 'ng-zorro-antd/checkbox';
import { ElectronService } from '../../core/electron.service';

@Component({
  selector: 'app-base64',
  imports: [FormsModule, NzButtonModule, NzCheckboxModule],
  templateUrl: './base64.component.html',
  styleUrl: './base64.component.scss'
})
export class Base64Component {
  inputValue: string = '';
  isWrapped: boolean = false;
  outputValue: string = ''; // Add this line to hold the output value

  constructor(private electronService: ElectronService) { } // Replace 'any' with the actual type if available

  decode() {
    console.log('decode');
  }
  encode() {
    console.log('encode');
    this.electronService.invoke('encodeBase64', { data: this.inputValue, wrap: this.isWrapped });
  }


}
