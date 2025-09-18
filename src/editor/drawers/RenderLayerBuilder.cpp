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
//  RenderLayerBuilder.cpp
//  arenaforge_editor
//
//  Created by king on 2025/9/18.
//

#include "RenderLayerBuilder.h"

#include <arenaforge_core/layers/Layer.h>
#include <arenaforge_core/layers/ShapeLayer.h>

#include <tgfx/layers/ShapeLayer.h>
#include <tgfx/layers/SolidColor.h>

namespace arenaforge {
std::shared_ptr<tgfx::Layer> RenderLayerBuilder::BuildFromData(const Layer *data) {
    if (data == nullptr) {
        return nullptr;
    }

    auto type = data->type();
    switch (type) {
        case LayerType::Group: {
            auto layer = tgfx::ShapeLayer::Make();
            layer->setName(data->layerId());
            return layer;
            break;
        }
        case LayerType::Shape: {
            return BuildShapeLayer(static_cast<const ShapeLayer *>(data));
            break;
        }

        default:
            break;
    }

    return nullptr;
}

std::shared_ptr<tgfx::ShapeLayer> RenderLayerBuilder::BuildShapeLayer(const ShapeLayer *data) {
    if (data == nullptr) {
        return nullptr;
    }
    auto layer = tgfx::ShapeLayer::Make();
    layer->setName(data->layerId());
    return layer;
}
    
};  // namespace arenaforge
