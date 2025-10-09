# ArenaForge 项目 AI 助手指南 (AGENTS.md)

本文件旨在为大型语言模型（LLM）提供关于 `ArenaForge` 项目的上下文信息，以帮助您更准确、高效地理解项目并提供相关辅助。

**请注意：你可以使用英文进行思考和分析，但最终的回答和代码实现必须以中文呈现。**

---

## 1. 项目概述

**ArenaForge** 是一个专为演出场馆设计的座位图与座位排布编辑器。项目核心目标是提供一个高性能、跨平台的编辑器内核，并结合原生 UI 框架，为用户提供流畅、直观的场馆布局设计体验。

- **核心功能**: 创建、编辑、管理场馆的座位布局。
- **目标平台**: 目前主要针对 macOS，但核心逻辑设计为跨平台。

---

## 2. 技术栈

- **核心逻辑**: C++
- **UI 框架**: macOS (AppKit / SwiftUI)
- **构建系统**: CMake
- **2D 渲染引擎**: `tgfx` (位于 `third_party`)
- **其他三方库**: `fmt` (格式化库)

---

## 3. 项目结构解析

```
├── cmake
│   ├── ios.toolchain.cmake
│   └── utils.cmake
├── CMakeLists.txt
├── DEPS
├── include
│   ├── arenaforge_core
│   └── arenaforge_editor
├── install_tools.sh
├── LICENSE
├── mac
│   ├── ArenaForgeStudio
│   ├── ArenaForgeStudio.xcworkspace
│   ├── gen_mac_xcode.sh
│   └── gen_xcode
├── README.md
├── src
│   ├── core
│   └── editor
├── sync_deps.sh
├── test
│   ├── base
│   └── core
├── third_party
│   ├── fmt
│   └── tgfx
└── XcodeTemplates
    └── ArenaForge
```


### 关键目录说明：

- **`src/core`**: 项目的数据模型层。这里定义了与具体渲染和 UI 无关的纯数据结构，例如场馆、区域、座位排、座位等。
- **`src/editor`**: 编辑器的核心业务逻辑。它依赖 `src/core` 中的数据结构，并使用 `third_party/tgfx` 渲染引擎将这些数据结构可视化地绘制出来，同时处理用户的交互事件（如选中、拖动等）。
- **`mac/`**: macOS 的 UI 层。这部分代码（Swift/Objective-C）负责创建窗口、菜单等原生 UI 元素，并嵌入一个由 `src/editor` 驱动的渲染视图。UI 层的操作会调用 C++ 层的接口来改变数据和触发重绘。
- **`DEPS` & `sync_deps.sh`**: 项目的依赖管理机制。所有第三方库都在 `DEPS` 文件中声明，通过执行 `sync_deps.sh` 脚本可以自动拉取或更新这些依赖。

---

## 4. 构建与运行流程

要成功编译和运行此项目，请遵循以下步骤：

1.  **同步依赖**:
    首次克隆项目或 `DEPS` 文件更新后，必须在项目根目录执行此命令来拉取所有第三方库。
    ```bash
    ./sync_deps.sh
    ```

2.  **生成 Xcode 项目**:
    执行 `mac` 目录下的脚本来生成 Xcode 工程文件。
    ```bash
    ./mac/gen_mac_xcode.sh
    ```
    该脚本会调用 CMake，在 `mac/` 目录下生成 `ArenaForgeStudio.xcworkspace`。

3.  **编译与运行**:
    使用 Xcode 打开 `mac/ArenaForgeStudio.xcworkspace`，选择对应的 Target（例如 `ArenaForgeStudio`），然后直接点击 "Build and Run" 即可。

---

## 5. 核心工作流

1.  **数据定义**: `src/core` 中定义了场馆布局的 C++ 数据结构。
2.  **渲染逻辑**: `src/editor` 中的 C++ 代码负责将这些数据结构转换为 `tgfx` 引擎可以识别的绘图指令，并将其渲染到一块画布上。
3.  **UI 交互**: `mac/` 目录下的 SwiftUI/AppKit 代码创建应用窗口，并提供一个视图（View）来承载 C++ 渲染的画布。用户的鼠标点击、拖拽等事件由 UI 层捕获，然后传递给 C++ 的 `editor` 核心进行处理。
4.  **数据流**: UI 操作 -> 调用 C++ 接口 -> `src/editor` 修改 `src/core` 数据 -> `src/editor` 通知渲染引擎重绘 -> 视图更新。
