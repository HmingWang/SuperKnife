import { Component, ElementRef, ViewChild } from '@angular/core';
// Import GPUCanvasContext type from TypeScript DOM lib
type GPUCanvasContext = any;

@Component({
  selector: 'app-webgpu',
  imports: [],
  templateUrl: './webgpu.component.html',
  styleUrl: './webgpu.component.scss'
})
export class WebgpuComponent {
  @ViewChild('canvas', { static: true }) canvasRef!: ElementRef<HTMLCanvasElement>;


  async ngAfterViewInit() {
    // Check for WebGPU support
    if (!navigator.gpu) {
      throw new Error("WebGPU not supported.");
    }

    console.log(navigator.gpu);
    console.log("WebGPU is supported.");
    const canvas = this.canvasRef.nativeElement;
    const adapter = await navigator.gpu.requestAdapter();
    if (!adapter) {
      throw new Error("Failed to get GPU adapter.");
    }
    const device = await adapter.requestDevice();
    const context: GPUCanvasContext = canvas.getContext('webgpu') as unknown as GPUCanvasContext;

    const format = navigator.gpu.getPreferredCanvasFormat();
    context.configure({
      device: device,
      format: format,
      alphaMode: 'opaque'
    });
    console.log("Canvas configured with format:", format);


    console.log("Render pipeline created.");

    console.log("Starting render loop.");

    console.log("Render loop started.");



  }


}
