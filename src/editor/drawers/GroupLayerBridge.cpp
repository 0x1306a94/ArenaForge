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
//  GroupLayerBridge.cpp
//  arenaforge_editor
//
//  Created by king on 2025/9/17.
//

#include "GroupLayerBridge.h"

#include <arenaforge_core/layers/Layer.h>

#include <tgfx/layers/ShapeLayer.h>

namespace arenaforge {
std::shared_ptr<GroupLayerBridge> GroupLayerBridge::Make(tgfx::Layer *renderParent, std::shared_ptr<Layer> data) {
    return std::shared_ptr<GroupLayerBridge>(new GroupLayerBridge(renderParent, std::move(data)));
}

GroupLayerBridge::GroupLayerBridge(tgfx::Layer *renderParent, std::shared_ptr<Layer> data)
    : LayerBridge(renderParent)
    , _data(std::move(data))
    , _renderLayer(nullptr) {
}

std::shared_ptr<Layer> GroupLayerBridge::dataLayer() {
    return _data;
}

std::shared_ptr<tgfx::Layer> GroupLayerBridge::renderLayer() {
    return std::static_pointer_cast<tgfx::Layer>(_renderLayer);
}

void GroupLayerBridge::buildRenderLayer() {
    if (_renderParent == nullptr) {
        return;
    }

    if (_renderLayer != nullptr) {
        return;
    }

    auto frame = _data->frame();
    auto renderLayer = tgfx::ShapeLayer::Make();
    tgfx::Path path;
    path.addRect(tgfx::Rect::MakeWH(frame.width(), frame.height()));
    renderLayer->setPath(std::move(path));
    renderLayer->setPosition({frame.x(), frame.y()});
    _renderLayer = std::move(renderLayer);

    _renderParent->addChild(_renderLayer);
}
};  // namespace arenaforge
