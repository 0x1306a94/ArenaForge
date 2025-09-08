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

#include <arenaforge_core/layers/BaseLayer.h>

#include <arenaforge_core/PathCommand.h>
#include <arenaforge_core/utils/UnitConverter.h>
#include <arenaforge_core/uuid/UUID.h>

#include "LayerStyleClone.h"
#include "ShapeStyleClone.h"

namespace arenaforge {
std::shared_ptr<BaseLayer> BaseLayer::Make(const std::string &layerId, ShapeType type) {
    return std::shared_ptr<BaseLayer>(new BaseLayer(layerId, type));
}

BaseLayer::BaseLayer(const std::string &layerId, ShapeType type)
    : tgfx::ShapeLayer()
    , _layerId(layerId) {

    initializePathCommand(type);
}

void BaseLayer::setTransient(bool value) {
    if (_transient == value) {
        return;
    }
    _transient = value;
    invalidateContent();
}

void BaseLayer::setPositionRelative(bool value) {
    if (_positionRelative == value) {
        return;
    }
    _positionRelative = value;
    invalidateContent();
}

void BaseLayer::setFrame(const tgfx::Rect &frame) {
    if (_frame == frame) {
        return;
    }
    _frame = frame;
    invalidateContent();
}

void BaseLayer::clearAttributes() {
    _attributes.clear();
}

void BaseLayer::addAttribute(const std::string &key, const std::string &value) {
    _attributes[key] = value;
}

std::shared_ptr<BaseLayer> BaseLayer::getChildById(const std::string &layerId) {
    for (const auto &child : children()) {
        if (auto base = std::dynamic_pointer_cast<BaseLayer>(child); base->layerId() == layerId) {
            return base;
        }
    }
    return nullptr;
}

std::shared_ptr<BaseLayer> BaseLayer::clone(bool cloneChildren) const {
    auto uuid = UUID::Instance();
    auto copied = BaseLayer::Make(uuid(), ShapeType::Rectangle);
    doClone(copied.get(), cloneChildren);
    return copied;
}

void BaseLayer::doClone(BaseLayer *target, bool cloneChildren) const {
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
    target->_pathCommands = _pathCommands;

    target->setFrame(frame());
    target->setTransient(transient());
    target->setPositionRelative(positionRelative());

    auto maskLayer = mask();
    if (maskLayer) {
        auto baseLayer = std::dynamic_pointer_cast<BaseLayer>(maskLayer);
        if (baseLayer) {
            auto copied = baseLayer->clone(cloneChildren);
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
                auto copied = baseLayer->clone(cloneChildren);
                target->addChild(copied);
            }
        }
    }
}

void BaseLayer::updatePathCommands() {
}

void BaseLayer::onUpdateContent(tgfx::LayerRecorder *recorder) {
    if (_positionRelative) {
        // rebuild path
        tgfx::Path path;
        auto size = frame().size();
        for (const auto &command : _pathCommands) {
            switch (command.type) {
                case PathCommandType::MoveTo: {
                    auto moveTo = command.cmd.moveTo;
                    auto p = moveTo.p * size;
                    path.moveTo(p);
                    break;
                }
                case PathCommandType::LineTo: {
                    auto lineTo = command.cmd.lineTo;
                    auto p = lineTo.p * size;
                    path.lineTo(p);
                    break;
                }
                case PathCommandType::QuadTo: {
                    auto quadTo = command.cmd.quadTo;
                    auto c = quadTo.c * size;
                    auto p = quadTo.p * size;
                    path.quadTo(c, p);
                    break;
                }
                case PathCommandType::CubicTo: {
                    auto cubicTo = command.cmd.cubicTo;
                    auto c1 = cubicTo.c1 * size;
                    auto c2 = cubicTo.c2 * size;
                    auto p = cubicTo.p * size;
                    path.cubicTo(c1, c2, p);
                    break;
                }
                case PathCommandType::ClosePath: {
                    path.close();
                    break;
                }

                default:
                    break;
            }
        }

        setPath(path);
    }

    tgfx::ShapeLayer::onUpdateContent(recorder);
}

void BaseLayer::initializePathCommand(ShapeType type) {
    switch (type) {
        case ShapeType::Rectangle: {
            _pathCommands.push_back(PathCommand::MakeMoveTo({0.0, 0.0}));
            _pathCommands.push_back(PathCommand::MakeLineTo({1.0, 0.0}));
            _pathCommands.push_back(PathCommand::MakeLineTo({1.0, 1.0}));
            _pathCommands.push_back(PathCommand::MakeLineTo({0.0, 1.0}));
            _pathCommands.push_back(PathCommand::MakeLineTo({0.0, 0.0}));
            _pathCommands.push_back(PathCommand::MakeClose());
            break;
        }
        case ShapeType::Triangle: {
            _pathCommands.push_back(PathCommand::MakeMoveTo({0.5, 0.0}));
            _pathCommands.push_back(PathCommand::MakeLineTo({1.0, 1.0}));
            _pathCommands.push_back(PathCommand::MakeLineTo({0.0, 1.0}));
            _pathCommands.push_back(PathCommand::MakeLineTo({0.5, 0.0}));
            _pathCommands.push_back(PathCommand::MakeClose());
            break;
        }

        default:
            break;
    }
}
};  // namespace arenaforge
