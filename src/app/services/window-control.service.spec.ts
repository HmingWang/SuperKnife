import { TestBed } from '@angular/core/testing';

import { WindowControlService } from './window-control.service';

describe('WindowControlService', () => {
  let service: WindowControlService;

  beforeEach(() => {
    TestBed.configureTestingModule({});
    service = TestBed.inject(WindowControlService);
  });

  it('should be created', () => {
    expect(service).toBeTruthy();
  });
});
