import { Injectable } from '@angular/core';
import { BehaviorSubject } from 'rxjs';

@Injectable({
  providedIn: 'root'
})
export class StatusService {


  public showMenuButton:BehaviorSubject<boolean>= new BehaviorSubject<boolean>(false);
  public isCollapsed:BehaviorSubject<boolean>= new BehaviorSubject<boolean>(false);

}
