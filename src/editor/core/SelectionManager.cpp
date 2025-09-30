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
//  SelectionManager.cpp
//  arenaforge_editor
//
//  Created by king on 2025/9/30.
//

#include <arenaforge_editor/core/SelectionManager.h>

#include <arenaforge_core/Print.h>
#include <arenaforge_core/layers/Layer.h>

#include "drawers/LayerTreeAdapter.h"

namespace arenaforge::editor {
std::shared_ptr<SelectionManager> SelectionManager::Make(std::shared_ptr<arenaforge::editor::LayerTreeAdapter> layerTreeAdapter) {
    return std::shared_ptr<SelectionManager>(new SelectionManager(std::move(layerTreeAdapter)));
}

SelectionManager::SelectionManager(std::shared_ptr<arenaforge::editor::LayerTreeAdapter> layerTreeAdapter)
    : _layerTreeAdapter(std::move(layerTreeAdapter)) {
}

SelectionManager::~SelectionManager() {
    PrintLog("%s", __PRETTY_FUNCTION__);
}

void SelectionManager::selectLayers(std::vector<std::shared_ptr<arenaforge::Layer>> targets) {
    if (targets.empty()) {
        _selectedLayers.clear();
        _layerTreeAdapter->setSelectedLayers({});
        _layerTreeAdapter->clearSelectionDisplay();
        return;
    }
    _selectedLayers = std::move(targets);
    _layerTreeAdapter->setSelectedLayers(_selectedLayers);
    _layerTreeAdapter->updateSelectionDisplay();
}

void SelectionManager::beginMove(float x, float y) {
    _beginMoveX = x;
    _beginMoveY = y;
    std::unordered_map<std::string, arenaforge::Rect> memoLayerFrames{};
    for (auto &layer : _selectedLayers) {
        auto key = layer->layerId();
        auto frame = layer->frame();
        memoLayerFrames.insert_or_assign(key, std::move(frame));
    }
    _memoLayerFrames = std::move(memoLayerFrames);
}

void SelectionManager::updateMove(float x, float y) {
    auto distanceX = x - _beginMoveX;
    auto distanceY = y - _beginMoveY;

    for (auto &layer : _selectedLayers) {
        auto key = layer->layerId();
        auto it = _memoLayerFrames.find(key);
        if (it == _memoLayerFrames.end()) {
            continue;
        }

        auto frame = it->second;
        frame.offset(distanceX, distanceY);
        layer->setFrame(frame);
    }
    _layerTreeAdapter->updateSelectionDisplay();
}

void SelectionManager::endMove() {
    _beginMoveX = 0.0;
    _beginMoveY = 0.0;
    _memoLayerFrames.clear();
}

void SelectionManager::clearSelection() {
    _selectedLayers.clear();
}

};  // namespace arenaforge::editor
