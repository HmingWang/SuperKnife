import { Component } from '@angular/core';
import { Router, RouterLink, RouterLinkActive } from '@angular/router';
import { NzBreadCrumbModule } from 'ng-zorro-antd/breadcrumb';
import { NzIconModule } from 'ng-zorro-antd/icon';
import { NzLayoutModule } from 'ng-zorro-antd/layout';
import { NzMenuModule } from 'ng-zorro-antd/menu';
import { FooterComponent } from "../../components/footer/footer.component";
import { RouterOutlet } from '@angular/router';
import { WindowControlService } from '../../services/window-control.service';
@Component({
  selector: 'app-dashboard',
  imports: [NzBreadCrumbModule, NzIconModule, NzLayoutModule, NzMenuModule, FooterComponent, RouterOutlet, RouterLink],
  templateUrl: './dashboard.component.html',
  styleUrl: './dashboard.component.scss'
})
export class DashboardComponent {
  constructor(private router: Router,private windowControlService:WindowControlService) {


  }

  isCollapsed = false;
  ngOnInit(): void {
    this.windowControlService.reset();
  }

}
