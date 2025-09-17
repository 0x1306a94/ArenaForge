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
//  RootLayerBridge.cpp
//  arenaforge_editor
//
//  Created by king on 2025/9/17.
//

#include "RootLayerBridge.h"

#include <arenaforge_core/layers/Layer.h>

#include <tgfx/layers/ShapeLayer.h>
#include <tgfx/layers/SolidColor.h>

namespace arenaforge {
std::shared_ptr<RootLayerBridge> RootLayerBridge::Make(tgfx::Layer *renderParent, std::shared_ptr<Layer> data) {
    return std::shared_ptr<RootLayerBridge>(new RootLayerBridge(renderParent, std::move(data)));
}

RootLayerBridge::RootLayerBridge(tgfx::Layer *renderParent, std::shared_ptr<Layer> data)
    : LayerBridge(renderParent)
    , _data(std::move(data))
    , _rootLayer(nullptr)
    , _containerLayer(nullptr)
    , _maskLayer(nullptr) {
}

std::shared_ptr<Layer> RootLayerBridge::dataLayer() {
    return std::static_pointer_cast<Layer>(_data);
}

std::shared_ptr<tgfx::Layer> RootLayerBridge::renderLayer() {
    return std::static_pointer_cast<tgfx::Layer>(_rootLayer);
}

void RootLayerBridge::buildRenderLayer() {
    if (_renderParent == nullptr) {
        return;
    }

    if (_rootLayer != nullptr) {
        return;
    }

    auto frame = _data->frame();

    _rootLayer = tgfx::ShapeLayer::Make();

    tgfx::Path rootPath;
    rootPath.addRect(tgfx::Rect::MakeWH(frame.width(), frame.height()));
    _rootLayer->setPath(rootPath);

    _containerLayer = tgfx::ShapeLayer::Make();
    _containerLayer->setPath(rootPath);
    _containerLayer->setFillStyle(tgfx::SolidColor::Make(tgfx::Color::FromRGBA(0xcc, 0xcc, 0xcc)));

    _maskLayer = tgfx::ShapeLayer::Make();
    _maskLayer->setPath(rootPath);
    _maskLayer->setFillStyle(tgfx::SolidColor::Make(tgfx::Color::FromRGBA(0xcc, 0xcc, 0xcc)));

    _rootLayer->addChild(_containerLayer);
    _rootLayer->addChild(_maskLayer);
    _containerLayer->setMask(_maskLayer);

    _renderParent->addChild(_rootLayer);
}
};  // namespace arenaforge
