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
//  HoverManager.cpp
//  arenaforge_editor
//
//  Created by king on 2025/10/9.
//

#include <arenaforge_editor/core/HoverManager.h>

#include <arenaforge_core/Print.h>
#include <arenaforge_core/layers/Layer.h>
#include <arenaforge_core/layers/ShapeLayer.h>

#include "renderer/LayerTreeAdapter.h"
#include "renderer/PathBuilder.h"

#include <tgfx/layers/Layer.h>
#include <tgfx/layers/ShapeLayer.h>
#include <tgfx/layers/SolidColor.h>

namespace arenaforge::editor {
std::unique_ptr<HoverManager> HoverManager::Make(arenaforge::editor::LayerTreeAdapter *layerTreeAdapter, std::shared_ptr<tgfx::Layer> overlayRoot, std::shared_ptr<tgfx::Layer> designRoot) {
    return std::unique_ptr<HoverManager>(new HoverManager(layerTreeAdapter, std::move(overlayRoot), std::move(designRoot)));
}

HoverManager::HoverManager(arenaforge::editor::LayerTreeAdapter *layerTreeAdapter, std::shared_ptr<tgfx::Layer> overlayRoot, std::shared_ptr<tgfx::Layer> designRoot)
    : _layerTreeAdapter(layerTreeAdapter)
    , _overlayRoot(std::move(overlayRoot))
    , _designRoot(std::move(designRoot)) {
}

HoverManager::~HoverManager() {
    PrintLog("%s", __PRETTY_FUNCTION__);
}

void HoverManager::addHoverWireframe(const std::vector<std::shared_ptr<arenaforge::Layer>> &targets) {
    if (targets.empty()) {
        return;
    }

    if (!_overlayRoot || !_designRoot) {
        return;
    }

    for (auto &target : targets) {
        auto layerId = target->layerId();
        auto it = _hoverWireframeLayers.find(layerId);
        if (it != _hoverWireframeLayers.end()) {
            continue;
        }

        std::shared_ptr<tgfx::Layer> renderLayer = _designRoot;
        if (!target->isRoot()) {
            renderLayer = _layerTreeAdapter->findRenderLayer(layerId);
        }

        if (!renderLayer) {
            continue;
        }

        // 先将自身转为全局，再从全局转为_rootLayer
        auto position = renderLayer->localToGlobal(tgfx::Point::Zero());
        if (!target->isRoot()) {
            position = _designRoot->globalToLocal(position);
        }

        auto frame = target->frame();

        auto hoverLayer = tgfx::ShapeLayer::Make();

        if (target->type() == arenaforge::LayerType::Shape) {
            auto shapeData = std::static_pointer_cast<arenaforge::ShapeLayer>(target);
            const auto &commands = shapeData->pathCommands();
            tgfx::Path path = PathBuilder::BuildPath(commands, frame.size());
            hoverLayer->setPath(std::move(path));
        } else {

            tgfx::Path path;
            path.addRect(tgfx::Rect::MakeWH(frame.width(), frame.height()));
            hoverLayer->setPath(std::move(path));
        }

        hoverLayer->setStrokeStyle(tgfx::SolidColor::Make(tgfx::Color::FromRGBA(0x0c, 0x8c, 0xe9)));
        hoverLayer->setPosition(position);
        hoverLayer->setLineWidth(2);
        hoverLayer->setStrokeAlign(tgfx::StrokeAlign::Outside);

        _overlayRoot->addChild(hoverLayer);

        _hoverWireframeLayers[layerId] = hoverLayer;
    }
}

void HoverManager::resetHoverWireframe() {
    if (_hoverWireframeLayers.empty()) {
        return;
    }
    std::unordered_map<std::string, std::weak_ptr<tgfx::Layer>> exists;
    std::swap(_hoverWireframeLayers, exists);
    for (auto &[key, value] : exists) {
        if (auto layer = value.lock()) {
            layer->removeFromParent();
        }
    }
}
};  // namespace arenaforge::editor
