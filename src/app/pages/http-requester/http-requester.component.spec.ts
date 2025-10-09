import { ComponentFixture, TestBed } from '@angular/core/testing';

import { HttpRequesterComponent } from './http-requester.component';

describe('HttpRequesterComponent', () => {
  let component: HttpRequesterComponent;
  let fixture: ComponentFixture<HttpRequesterComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [HttpRequesterComponent]
    })
    .compileComponents();

    fixture = TestBed.createComponent(HttpRequesterComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
