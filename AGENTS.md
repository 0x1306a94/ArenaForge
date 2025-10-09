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

## 3. 项目结构与核心组件解析

```
.
├── src
│   ├── core (数据模型层)
│   └── editor (编辑器核心逻辑层)
├── mac (macOS 原生 UI 层)
├── include
│   ├── arenaforge_core
│   └── arenaforge_editor
...
```

### 关键组件说明：

- **`src/core`**: **纯数据模型层**。这里定义了与具体渲染和 UI 无关的数据结构，例如 `Project`、`Layer`、`ShapeLayer` 等。这是整个项目的基石，完全独立。

- **`src/editor`**: **编辑器的核心业务逻辑层**。这是应用的“大脑”，由多个高内聚的组件构成：
    - **`Editor`**: 编辑器的顶层**协调者 (Coordinator)** 和对外的**门面 (Facade)**。它拥有并管理所有核心组件的生命周期，并为 `mac/` UI 层提供一个稳定、高级的 API 接口。UI 层只应与 `Editor` 类交互。
    - **`renderer/Renderer`**: 纯粹的**渲染管理者**。它拥有并管理唯一的 `tgfx::DisplayList` 实例，并在内部构建了一个结构化的场景图（例如，分为 `designLayer` 用于内容绘制，`overlayLayer` 用于 UI 装饰）。它只负责最终的渲染命令执行，不包含任何业务逻辑。
    - **`renderer/LayerTreeAdapter`**: **数据-视图适配器**。这是连接 `src/core` 数据模型和 `tgfx` 渲染树的关键桥梁。它监听数据层的变化，并自动、高效地将这些变化同步到 `Renderer` 所管理的渲染层 (`tgfx::Layer`)。
    - **`core/SelectionManager`**: **选择管理器**。一个独立的逻辑单元，专门负责处理与对象“选中”相关的所有逻辑，包括计算选中框、更新其在 `overlayLayer` 上的视觉表现，以及处理选中对象的拖拽移动等。
    - **`core/HoverManager`**: **悬停管理器**。与 `SelectionManager` 类似，它负责处理鼠标悬停在对象上时的视觉反馈（例如高亮线框）。

- **`mac/`**: **macOS 的 UI 层**。这部分代码（Swift/Objective-C）负责创建窗口、菜单、属性面板等原生 UI 元素。它通过一个**桥接层 (Bridge)** 调用 `Editor` 提供的 C++ API 来响应用户操作和更新视图。**它完全不了解 `tgfx` 的存在**。

- **`DEPS` & `sync_deps.sh`**: 项目的依赖管理机制。

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

## 5. 核心工作流 (数据驱动)

项目的核心是一个清晰的、单向数据驱动的工作流：

1.  **场景初始化**: `Editor` 创建 `Renderer`，`Renderer` 内部构建好分层的 `DisplayList` 场景图。接着，`Editor` 创建 `LayerTreeAdapter`、`SelectionManager` 等核心组件，并将它们与 `Renderer` 的不同图层关联起来。

2.  **数据-视图同步**: `LayerTreeAdapter` 持续监听 `src/core` 数据模型。任何对数据的修改（例如 `layer->setFrame(...)`）都会被它捕获，并自动更新到 `tgfx` 渲染树中对应的 `tgfx::Layer` 节点。

3.  **UI 交互**:
    - `mac/` UI 层捕获用户点击事件，并调用 `editor->findDataLayerAtPoint(x, y)`。
    - `Editor` 内部通过 `Renderer` 和 `LayerTreeAdapter` 找到对应的 `arenaforge::Layer` **数据对象**并返回给 UI 层。
    - UI 层通知 `Editor` 更新选中状态，`Editor` 将任务委托给 `SelectionManager`。

4.  **状态更新与渲染**:
    - `SelectionManager` 接收到新的选中数据对象列表。它通过 `LayerTreeAdapter` 查询这些对象对应的渲染层，计算出它们在屏幕上的包围盒，然后在 `Renderer` 的 `overlayLayer` 上创建或更新一个 `tgfx::ShapeLayer` 来显示选中框。
    - 如果用户拖拽对象，`SelectionManager` 会直接修改 `arenaforge::Layer` 的数据属性（如 `frame`）。
    - 这个数据修改会**自动触发**第 2 步的同步流程，`LayerTreeAdapter` 将更新渲染层的位置。
    - 主循环调用 `editor->draw()`，`Renderer` 将包含所有最新状态的 `DisplayList` 高效地渲染到屏幕上。

这个架构实现了高度的**关注点分离**，使得数据、逻辑和视图之间的关系清晰明了，易于维护和扩展。