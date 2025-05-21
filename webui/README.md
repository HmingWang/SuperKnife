# Webui

This project was generated using [Angular CLI](https://github.com/angular/angular-cli) version 19.2.12.

## Development server

To start a local development server, run:

```bash
ng serve
```

Once the server is running, open your browser and navigate to `http://localhost:4200/`. The application will automatically reload whenever you modify any of the source files.

## Code scaffolding

Angular CLI includes powerful code scaffolding tools. To generate a new component, run:

```bash
ng generate component component-name
```

For a complete list of available schematics (such as `components`, `directives`, or `pipes`), run:

```bash
ng generate --help
```

## Building

To build the project run:

```bash
ng build
```

This will compile your project and store the build artifacts in the `dist/` directory. By default, the production build optimizes your application for performance and speed.

## Running unit tests

To execute unit tests with the [Karma](https://karma-runner.github.io) test runner, use the following command:

```bash
ng test
```

## Running end-to-end tests

For end-to-end (e2e) testing, run:

```bash
ng e2e
```

Angular CLI does not come with an end-to-end testing framework by default. You can choose one that suits your needs.

## Additional Resources

For more information on using the Angular CLI, including detailed command references, visit the [Angular CLI Overview and Command Reference](https://angular.dev/tools/cli) page.

## ng 目录树
```
my-angular-app/
├── e2e/                  # 端到端测试文件
├── node_modules/         # 第三方依赖包
├── src/                  # 主要源代码目录
│   ├── app/              # 应用模块和组件
│   │   ├── components/   # 通用组件
│   │   ├── pages/        # 页面级组件
│   │   ├── services/     # 服务
│   │   ├── models/       # 数据模型
│   │   ├── guards/       # 路由守卫
│   │   ├── interceptors/ # HTTP拦截器
│   │   ├── pipes/        # 自定义管道
│   │   ├── directives/   # 自定义指令
│   │   ├── utils/        # 工具函数
│   │   ├── shared/       # 共享模块
│   │   ├── core/         # 核心模块(单例服务等)
│   │   ├── app.module.ts # 根模块
│   │   └── app-routing.module.ts # 根路由
│   ├── assets/           # 静态资源(图片、字体等)
│   ├── environments/     # 环境配置
│   ├── styles/           # 全局样式
│   ├── index.html        # 主HTML文件
│   └── main.ts           # 应用入口文件
├── .gitignore            # Git忽略规则
├── angular.json          # Angular CLI配置
├── package.json          # 项目配置和依赖
└── tsconfig.json         # TypeScript配置
```
