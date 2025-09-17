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

#include <arenaforge_editor/core/Platform.h>
#include <arenaforge_editor/core/defines.h>

#include <memory>
#include <vector>

namespace tgfx {
class ShapeLayer;
class TextLayer;
}  // namespace tgfx

namespace arenaforge {
class Project;
class Venue;
class LayerBridgeManager;
};  // namespace arenaforge

namespace arenaforge::editor {
class RendererBackend;
class Renderer;
class ARENA_FORGE_EXPORT_API Editor : public std::enable_shared_from_this<Editor> {
  public:
    static std::shared_ptr<Editor> Make(std::shared_ptr<arenaforge::Project> project);
    ~Editor();

    void setRendererBackend(std::shared_ptr<RendererBackend> rendererBackend);

    bool getBoundsSize(float &width, float &height) const;

    float density() const;

    /// 当前缩放比例
    float zoomScale() const;

    /// 当前滑动偏移
    bool contentOffset(float &x, float &y) const;

    bool updateZoomAndOffset(float zoomScale, float offsetX, float offsetY);

    bool updateSize();

    void autoAdjustCanvasScaleForContent();

    void invalidateContent();

    void draw(bool force = false);

  protected:
    Editor(std::shared_ptr<arenaforge::Project> project);

  private:
    void setupRootLayer();

  private:
    std::shared_ptr<arenaforge::Project> _project{nullptr};
    std::shared_ptr<Renderer> _renderer{nullptr};
    std::shared_ptr<LayerBridgeManager> _bridgeManager{nullptr};
};
};  // namespace arenaforge::editor

#endif /* Editor_h_ArenaForge */
