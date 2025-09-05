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
//  BaseLayer.cpp
//  arenaforge_core
//
//  Created by king on 2025/9/5.
//

#include <arenaforge/core/layers/BaseLayer.h>

#include <arenaforge/core/IdGenerator.h>

#include "LayerStyleClone.h"
#include "ShapeStyleClone.h"

namespace arenaforge {
std::shared_ptr<BaseLayer> BaseLayer::Make(uint32_t layerId) {
    return std::shared_ptr<BaseLayer>(new BaseLayer(layerId));
}

BaseLayer::BaseLayer(uint32_t layerId)
    : tgfx::ShapeLayer()
    , _layerId(layerId) {
}

void BaseLayer::setTransient(bool value) {
    if (_transient == value) {
        return;
    }
    _transient = value;
    invalidateContent();
}

std::shared_ptr<BaseLayer> BaseLayer::getChildById(uint32_t layerId) {
    for (const auto &child : children()) {
        if (auto base = std::dynamic_pointer_cast<BaseLayer>(child); base->layerId() == layerId) {
            return base;
        }
    }
    return nullptr;
}

std::shared_ptr<BaseLayer> BaseLayer::clone(IdGenerator *idGen, bool cloneChildren) const {
    auto copied = BaseLayer::Make(idGen->generate());
    doClone(copied.get(), idGen, cloneChildren);
    return copied;
}

void BaseLayer::doClone(BaseLayer *target, IdGenerator *idGen, bool cloneChildren) const {
    target->setName(name());
    target->setAlpha(alpha());
    target->setVisible(visible());
    target->setBlendMode(blendMode());
    target->setMatrix(matrix());
    target->setShouldRasterize(shouldRasterize());
    target->setRasterizationScale(rasterizationScale());
    target->setAllowsGroupOpacity(allowsGroupOpacity());
    target->setAllowsEdgeAntialiasing(allowsEdgeAntialiasing());
    target->setExcludeChildEffectsInLayerStyle(excludeChildEffectsInLayerStyle());

    auto maskLayer = mask();
    if (maskLayer) {
        auto baseLayer = std::dynamic_pointer_cast<BaseLayer>(maskLayer);
        if (baseLayer) {
            auto copied = baseLayer->clone(idGen, cloneChildren);
            target->setMask(copied);
        }
    }
    target->setMaskType(maskType());
    target->setScrollRect(scrollRect());

    auto styles = layerStyles();
    if (!styles.empty()) {
        std::vector<std::shared_ptr<tgfx::LayerStyle>> copiedStyles;
        for (const auto &style : styles) {
            auto copied = cloneLayerStyle(style.get());
            if (copied) {
                copiedStyles.push_back(copied);
            }
        }
        target->setLayerStyles(std::move(copiedStyles));
    }

    target->setPath(path());
    target->setLineWidth(lineWidth());
    target->setMiterLimit(miterLimit());
    target->setStrokeStart(strokeStart());
    target->setStrokeEnd(strokeEnd());
    target->setStrokeAlign(strokeAlign());
    target->setStrokeOnTop(strokeOnTop());
    target->setLineCap(lineCap());
    target->setLineJoin(lineJoin());
    target->setLineDashPhase(lineDashPhase());
    target->setLineDashPattern(lineDashPattern());
    target->setLineDashAdaptive(lineDashAdaptive());

    auto fillStyles = this->fillStyles();
    auto strokeStyles = this->strokeStyles();
    if (!fillStyles.empty()) {
        std::vector<std::shared_ptr<tgfx::ShapeStyle>> copiedStyles;
        for (const auto &style : fillStyles) {
            auto copied = cloneShapeStyle(style.get());
            if (copied) {
                copiedStyles.push_back(copied);
            }
        }
        target->setFillStyles(std::move(copiedStyles));
    }

    if (!strokeStyles.empty()) {
        std::vector<std::shared_ptr<tgfx::ShapeStyle>> copiedStyles;
        for (const auto &style : strokeStyles) {
            auto copied = cloneShapeStyle(style.get());
            if (copied) {
                copiedStyles.push_back(copied);
            }
        }
        target->setStrokeStyles(std::move(copiedStyles));
    }

    if (cloneChildren) {
        auto children = this->children();
        for (auto &child : children) {
            auto baseLayer = std::dynamic_pointer_cast<BaseLayer>(child);
            if (baseLayer) {
                auto copied = baseLayer->clone(idGen, cloneChildren);
                target->addChild(copied);
            }
        }
    }
}

};  // namespace arenaforge
