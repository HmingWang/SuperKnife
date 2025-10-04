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

    // 创建渲染管线
    const pipeline = device.createRenderPipeline({
      vertex: {
        module: device.createShaderModule({
          code: `
          @vertex
          fn main(@builtin(vertex_index) VertexIndex : u32) -> @builtin(position) vec4<f32> {
            var pos = array<vec2<f32>, 3>(
              vec2<f32>(0.0, 0.5),
              vec2<f32>(-0.5, -0.5),
              vec2<f32>(0.5, -0.5)
            );
            return vec4<f32>(pos[VertexIndex], 0.0, 1.0);
          }`
        }),
        entryPoint: 'main'
      },
      fragment: {
        module: device.createShaderModule({
          code: `
          @fragment
          fn main() -> @location(0) vec4<f32> {
            return vec4<f32>(1.0, 0.0, 0.0, 1.0);
          }`
        }),
        entryPoint: 'main',
        targets: [{
          format: format
        }]
      },
      primitive: {
        topology: 'triangle-list'
      }
    });

    console.log("Render pipeline created.");

    // 渲染循环
    const render = () => {
      const encoder = device.createCommandEncoder();
      const pass = encoder.beginRenderPass({
        colorAttachments: [
          {
            view: context.getCurrentTexture().createView(),
            loadOp: 'clear',
            storeOp: 'store',
            clearValue: { r: 0.1, g: 0.1, b: 0.15, a: 1.0 },
          },
        ],
      });
      pass.setPipeline(pipeline);
      pass.draw(3);
      pass.end();
      device.queue.submit([encoder.finish()]);
      requestAnimationFrame(render);
    };
    console.log("Starting render loop.");

    render();
    console.log("Render loop started.");



  }


}
