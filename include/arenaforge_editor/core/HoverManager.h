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
//  HoverManager.h
//  arenaforge_editor
//
//  Created by king on 2025/10/9.
//

#ifndef HoverManager_h_ArenaForge
#define HoverManager_h_ArenaForge

#include <memory>
#include <string>
#include <unordered_map>

namespace tgfx {
class Layer;
};  // namespace tgfx

namespace arenaforge {
class Layer;
};

namespace arenaforge::editor {
class LayerTreeAdapter;
class HoverManager {
  public:
    static std::unique_ptr<HoverManager> Make(arenaforge::editor::LayerTreeAdapter *layerTreeAdapter, std::shared_ptr<tgfx::Layer> overlayRoot, std::shared_ptr<tgfx::Layer> designRoot);
    ~HoverManager();

    /// 添加hover指示框
    /// - Parameter targets: 需要添加的Layer
    void addHoverWireframe(const std::vector<std::shared_ptr<arenaforge::Layer>> &targets);

    /// 清除hover指示框
    void resetHoverWireframe();

  protected:
    HoverManager(arenaforge::editor::LayerTreeAdapter *layerTreeAdapter, std::shared_ptr<tgfx::Layer> overlayRoot, std::shared_ptr<tgfx::Layer> designRoot);

  private:
    arenaforge::editor::LayerTreeAdapter *_layerTreeAdapter{nullptr};
    std::shared_ptr<tgfx::Layer> _overlayRoot{nullptr};
    std::shared_ptr<tgfx::Layer> _designRoot{nullptr};
    std::unordered_map<std::string, std::weak_ptr<tgfx::Layer>> _hoverWireframeLayers{};
};
};  // namespace arenaforge::editor

#endif /* HoverManager_h_ArenaForge */
