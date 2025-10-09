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

#include "renderer/LayerTreeAdapter.h"

#include <tgfx/layers/ShapeLayer.h>
#include <tgfx/layers/SolidColor.h>

namespace arenaforge::editor {
std::shared_ptr<SelectionManager> SelectionManager::Make(arenaforge::editor::LayerTreeAdapter *layerTreeAdapter, std::shared_ptr<tgfx::Layer> overlayRoot) {
    return std::shared_ptr<SelectionManager>(new SelectionManager(layerTreeAdapter, std::move(overlayRoot)));
}

SelectionManager::SelectionManager(arenaforge::editor::LayerTreeAdapter *layerTreeAdapter, std::shared_ptr<tgfx::Layer> overlayRoot)
    : _layerTreeAdapter(layerTreeAdapter)
    , _overlayRoot(std::move(overlayRoot)) {
}

SelectionManager::~SelectionManager() {
    PrintLog("%s", __PRETTY_FUNCTION__);
}

void SelectionManager::updateSelection(const std::vector<std::shared_ptr<arenaforge::Layer>> &targets) {
    _selectedLayers = targets;
    if (_selectedLayers.empty()) {
        clearSelectionDisplay();
    } else {
        updateSelectionDisplay();
    }
}

void SelectionManager::clearSelection() {
    _selectedLayers.clear();
    clearSelectionDisplay();
}

void SelectionManager::updateSelectionDisplay() {
    if (_selectedLayers.empty() || !_layerTreeAdapter) {
        clearSelectionDisplay();
        return;
    }

    tgfx::Point min{FLT_MAX, FLT_MAX}, max{FLT_MIN, FLT_MIN};
    for (const auto &data : _selectedLayers) {
        auto renderLayer = _layerTreeAdapter->findRenderLayer(data->layerId());
        if (!renderLayer) {
            continue;
        }

        auto frame = data->frame();
        auto topLeft = renderLayer->localToGlobal({0.0, 0.0});
        auto bottomRight = renderLayer->localToGlobal({frame.width(), frame.height()});
        min.x = std::min(min.x, topLeft.x);
        min.y = std::min(min.y, topLeft.y);
        max.x = std::max(max.x, bottomRight.x);
        max.y = std::max(max.y, bottomRight.y);
    }

    if (!_selectedBoundingBoxLayer) {
        _selectedBoundingBoxLayer = tgfx::ShapeLayer::Make();
        _selectedBoundingBoxLayer->setName("SelectionBoundingBox");
        _selectedBoundingBoxLayer->setStrokeStyle(tgfx::SolidColor::Make(tgfx::Color::FromRGBA(0x0c, 0x8c, 0xe9)));
        _selectedBoundingBoxLayer->setLineWidth(2);
        _selectedBoundingBoxLayer->setStrokeAlign(tgfx::StrokeAlign::Outside);
        _overlayRoot->addChild(_selectedBoundingBoxLayer);
    }

    tgfx::Path boundingBoxPath;
    boundingBoxPath.addRect(min.x, min.y, max.x, max.y);
    _selectedBoundingBoxLayer->setPath(std::move(boundingBoxPath));
}

void SelectionManager::clearSelectionDisplay() {
    if (_selectedBoundingBoxLayer) {
        _selectedBoundingBoxLayer->removeFromParent();
        _selectedBoundingBoxLayer = nullptr;
    }
}

bool SelectionManager::hitTestPointInSelectedBoundingBox(float x, float y) const {
    if (!_selectedBoundingBoxLayer) {
        return false;
    }
    return _selectedBoundingBoxLayer->hitTestPoint(x, y);
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
    updateSelectionDisplay();
}

void SelectionManager::endMove() {
    _beginMoveX = 0.0;
    _beginMoveY = 0.0;
    _memoLayerFrames.clear();
}

};  // namespace arenaforge::editor
