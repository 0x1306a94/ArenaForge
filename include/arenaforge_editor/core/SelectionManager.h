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
//  SelectionManager.h
//  arenaforge_editor
//
//  Created by king on 2025/9/30.
//

#ifndef SelectionManager_h_ArenaForge
#define SelectionManager_h_ArenaForge

#include <memory>
#include <unordered_map>
#include <vector>

#include <arenaforge_core/Rect.h>

namespace tgfx {
class Layer;
class ShapeLayer;
};  // namespace tgfx

namespace arenaforge {
class Layer;
};

namespace arenaforge::editor {
class LayerTreeAdapter;
class SelectionManager : public std::enable_shared_from_this<SelectionManager> {
  public:
    static std::shared_ptr<SelectionManager> Make(arenaforge::editor::LayerTreeAdapter *layerTreeAdapter, std::shared_ptr<tgfx::Layer> overlayRoot);
    ~SelectionManager();

    void updateSelection(const std::vector<std::shared_ptr<arenaforge::Layer>> &targets);
    
    void clearSelection();
    
    void updateSelectionDisplay();
    
    void clearSelectionDisplay();
    
    bool hitTestPointInSelectedBoundingBox(float x, float y) const;

    void beginMove(float x, float y);
    void updateMove(float x, float y);
    void endMove();

  protected:
    SelectionManager(arenaforge::editor::LayerTreeAdapter *layerTreeAdapter, std::shared_ptr<tgfx::Layer> overlayRoot);

  private:
    arenaforge::editor::LayerTreeAdapter *_layerTreeAdapter{nullptr};
    std::shared_ptr<tgfx::Layer> _overlayRoot;
    std::shared_ptr<tgfx::ShapeLayer> _selectedBoundingBoxLayer{nullptr};
    float _beginMoveX{0.0};
    float _beginMoveY{0.0};
    std::vector<std::shared_ptr<arenaforge::Layer>> _selectedLayers{};
    std::unordered_map<std::string, arenaforge::Rect> _memoLayerFrames{};
};
};  // namespace arenaforge::editor

#endif /* SelectionManager_h_ArenaForge */
