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
//  LayerBridgeManager.cpp
//  arenaforge_editor
//
//  Created by king on 2025/9/17.
//

#include "LayerBridgeManager.h"

#include "GroupLayerBridge.h"
#include "LayerBridge.h"
#include "RootLayerBridge.h"
#include "ShapeLayerBridge.h"

#include <arenaforge_core/layers/Layer.h>
#include <arenaforge_core/layers/ShapeLayer.h>

#include <tgfx/layers/Layer.h>

namespace arenaforge {

std::shared_ptr<LayerBridge> LayerBridgeManager::createBridge(tgfx::Layer *renderParent, std::shared_ptr<Layer> layer) {
    if (layer == nullptr) {
        return nullptr;
    }
    std::shared_ptr<LayerBridge> bridge;
    if (layer->isRoot()) {
        bridge = RootLayerBridge::Make(renderParent, layer);
    } else {
        auto type = layer->type();
        switch (type) {
            case LayerType::Group: {
                bridge = GroupLayerBridge::Make(renderParent, layer);
                break;
            }
            case LayerType::Shape: {
                bridge = ShapeLayerBridge::Make(renderParent, std::static_pointer_cast<ShapeLayer>(layer));
                break;
            }
            default:
                break;
        }
    }

    if (bridge) {
        _bridges[layer->layerId()] = bridge;
    }
    return bridge;
}

std::shared_ptr<LayerBridge> LayerBridgeManager::getBridge(const std::string &layerId) const {
    auto it = _bridges.find(layerId);
    if (it == _bridges.end()) {
        return nullptr;
    }
    return it->second;
}

void LayerBridgeManager::syncAll() {
    for (auto &[_, bridge] : _bridges) {
        bridge->sync();
    }
}
};  // namespace arenaforge
