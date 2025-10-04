import { ComponentFixture, TestBed } from '@angular/core/testing';

import { WebgpuComponent } from './webgpu.component';

describe('WebgpuComponent', () => {
  let component: WebgpuComponent;
  let fixture: ComponentFixture<WebgpuComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [WebgpuComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(WebgpuComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
