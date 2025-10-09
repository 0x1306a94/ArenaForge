/////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ArenaForge - An open source venue editing and rendering framework
//
//  Copyright (c) 2025 0x1306a94 (https://github.com/0x1306a94)
//  All rights reserved.
//
//  Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
//  in compliance with the License. You may obtain a copy of the License at:
//
//      https://opensource.org/licenses/BSD-3-Clause
//
//  Unless required by applicable law or agreed to in writing, software distributed under the
//  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
//  either express or implied. See the License for the specific language governing permissions
//  and limitations under the License.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//
//  Editor.h
//  arenaforge_editor
//
//  Created by KK on 2025/9/6.
//

#ifndef Editor_h_ArenaForge
#define Editor_h_ArenaForge

#include <arenaforge_core/Point.h>
#include <arenaforge_core/Size.h>
#include <arenaforge_editor/core/Platform.h>
#include <arenaforge_editor/core/defines.h>

#include <memory>
#include <unordered_map>
#include <vector>

namespace tgfx {
class Layer;
class ShapeLayer;
class TextLayer;
}  // namespace tgfx

namespace arenaforge {
class Layer;
class Project;
class Venue;

};  // namespace arenaforge

namespace arenaforge::editor {
class RendererBackend;
class Renderer;
class LayerTreeAdapter;
class SelectionManager;
class HoverManager;
class ARENA_FORGE_EXPORT_API Editor : public std::enable_shared_from_this<Editor> {
  public:
    /// 创建编辑器
    /// - Parameter project: 工程实例
    static std::shared_ptr<Editor> Make(std::shared_ptr<arenaforge::Project> project);

    ~Editor();

    /// 设置渲染后端，应对于某些平台需要在某些时机后才能正确创建对应的渲染后端
    /// - Parameter rendererBackend: 渲染后端
    void setRendererBackend(std::shared_ptr<RendererBackend> rendererBackend);

    /// 选择管理器
    SelectionManager *selectionManager() {
        return _selectionManager.get();
    }

    /// Hover管理器
    HoverManager *hoverManager() {
        return _hoverManager.get();
    }

    /// 当前画布大小
    arenaforge::Size getBoundsSize() const;

    /// 当前显示密度
    float density() const;

    /// 当前缩放比例
    float zoomScale() const;

    /// 当前滑动偏移
    arenaforge::Point contentOffset() const;

    /// 更新缩放和位移
    /// - Parameters:
    ///   - zoomScale: 缩放
    ///   - offsetX: x方向位移
    ///   - offsetY: y方向位移
    bool updateZoomAndOffset(float zoomScale, float offsetX, float offsetY);

    /// 更新画布大小
    bool updateSize();

    /// 自动缩放内容到画布中心，以便于完整显示内容
    void autoAdjustCanvasScaleForContent();

    /// 检查当前点是否在容器内
    /// - Parameters:
    ///   - x: x 坐标
    ///   - y: y 坐标
    ///   - shapeHitTest: true 时精确查找 false 时近检查包围盒
    bool hitTestPointInContainer(float x, float y, bool shapeHitTest = false) const;

    /// 根据坐标查找Layer
    /// - Parameters:
    ///   - x: x 坐标
    ///   - y: y 坐标
    ///   - shapeHitTest: true 时精确查找 false 时近检查包围盒
    std::shared_ptr<arenaforge::Layer> findLayerAtPoint(float x, float y, bool shapeHitTest = false) const;

    /// 根据layerId 查找Layer
    /// - Parameter layerId: layerId
    std::shared_ptr<arenaforge::Layer> getLayerByLayerId(const std::string &layerId) const;

    /// 将全局坐标转为本地坐标
    /// - Parameters:
    ///   - global: 全局坐标
    ///   - sourceLayer: 需要转换到本地坐标的Layer
    std::optional<arenaforge::Point> globalToLocal(const arenaforge::Point &global, const std::shared_ptr<arenaforge::Layer> &sourceLayer) const;

    /// 将本地坐标转为全局坐标
    /// - Parameters:
    ///   - local: 本地坐标
    ///   - sourceLayer: 本地坐标对应的Layer
    std::optional<arenaforge::Point> localToGlobal(const arenaforge::Point &local, const std::shared_ptr<arenaforge::Layer> &sourceLayer) const;

    /// 标记内容无效
    void invalidateContent();

    /// 执行绘制
    /// - Parameters:
    ///   - force: 是否强制绘制
    void draw(bool force = false);

  protected:
    Editor(std::shared_ptr<arenaforge::Project> project);

  private:
    void setupRootLayer();
    void setupProjectSynchronization();
    void handleProjectCanvasSizeChanged();

  private:
    std::shared_ptr<arenaforge::Project> _project{nullptr};
    std::shared_ptr<Renderer> _renderer{nullptr};
    std::shared_ptr<tgfx::ShapeLayer> _rootLayer{nullptr};
    std::shared_ptr<tgfx::ShapeLayer> _containerLayer{nullptr};
    std::shared_ptr<tgfx::ShapeLayer> _maskLayer{nullptr};
    std::unique_ptr<LayerTreeAdapter> _treeAdapter{nullptr};
    std::unique_ptr<SelectionManager> _selectionManager{nullptr};
    std::unique_ptr<HoverManager> _hoverManager{nullptr};
};
};  // namespace arenaforge::editor

#endif /* Editor_h_ArenaForge */
