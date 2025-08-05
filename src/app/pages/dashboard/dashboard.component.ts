import { Component } from '@angular/core';
import { Router, RouterLink, RouterLinkActive } from '@angular/router';
import { NzBreadCrumbModule } from 'ng-zorro-antd/breadcrumb';
import { NzIconModule } from 'ng-zorro-antd/icon';
import { NzLayoutModule } from 'ng-zorro-antd/layout';
import { NzMenuModule } from 'ng-zorro-antd/menu';
import { FooterComponent } from "../../components/footer/footer.component";
import { RouterOutlet } from '@angular/router';
import { StatusService } from '../../services/status.service';
@Component({
  selector: 'app-dashboard',
  imports: [NzBreadCrumbModule, NzIconModule, NzLayoutModule, NzMenuModule, FooterComponent, RouterOutlet, RouterLink],
  templateUrl: './dashboard.component.html',
  styleUrl: './dashboard.component.scss'
})
export class DashboardComponent {
  constructor(private router: Router, private statusService: StatusService) {


  }

  isCollapsed = false;
  ngOnInit(): void {
    this.statusService.showMenuButton.next(true); // Show menu button on dashboard
    this.statusService.isCollapsed.asObservable().subscribe((collapsed: boolean) => {
      this.isCollapsed = collapsed;
    });
  }

}
