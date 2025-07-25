import { Component } from '@angular/core';
import { Router } from '@angular/router';

@Component({
  selector: 'app-base64',
  imports: [],
  templateUrl: './base64.component.html',
  styleUrl: './base64.component.scss'
})
export class Base64Component {
  constructor(private router: Router) { 
    console.log('Base64Component initialized');
    console.log('Router:', this.router.url);
  }
}
