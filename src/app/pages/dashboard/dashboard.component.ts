import { Component } from '@angular/core';
import { Router } from '@angular/router';
import { NzBreadCrumbModule } from 'ng-zorro-antd/breadcrumb';
import { NzIconModule } from 'ng-zorro-antd/icon';
import { NzLayoutModule } from 'ng-zorro-antd/layout';
import { NzMenuModule } from 'ng-zorro-antd/menu';
import { FooterComponent } from "../../components/footer/footer.component";
import { RouterOutlet } from '@angular/router';
@Component({
  selector: 'app-dashboard',
  imports: [NzBreadCrumbModule, NzIconModule, NzLayoutModule, NzMenuModule, FooterComponent, RouterOutlet],
  templateUrl: './dashboard.component.html',
  styleUrl: './dashboard.component.scss'
})
export class DashboardComponent {
  constructor(private router: Router) {}

  isCollapsed = false;


}
