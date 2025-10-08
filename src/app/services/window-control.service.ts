import { Injectable, signal } from '@angular/core';


export interface WindowControlOptions {
  backgroundColor?: string;
  color?: string;
  showCloseButton?: boolean;
  showMaximizeButton?: boolean;
  showMinimizeButton?: boolean;
  showMenuButton?: boolean;
  title?: string;
  collapsible?: boolean;
  transparent?: boolean;
}


@Injectable({
  providedIn: 'root'
})
export class WindowControlService {
  readonly options = signal<WindowControlOptions>({
    backgroundColor: '#001529',
    color: '#ffffff',
    showCloseButton: true,
    showMaximizeButton: true,
    showMinimizeButton: true,
    showMenuButton: false,
    title: 'Application',
    collapsible: false,
    transparent: false
  });

  setOptions(partial: Partial<WindowControlOptions>) {
    this.options.update(opt => ({ ...opt, ...partial }));
  }

  reset(){
    this.options.set({
      backgroundColor: '#001529',
      color: '#ffffff',
      showCloseButton: true,
      showMaximizeButton: true,
      showMinimizeButton: true,
      showMenuButton: false,
      title: 'Application',
      collapsible: false,
      transparent: false
    });
  }
}
