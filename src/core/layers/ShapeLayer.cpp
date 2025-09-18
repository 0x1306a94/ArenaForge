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
//  ShapeLayer.cpp
//  arenaforge_core
//
//  Created by king on 2025/9/17.
//

#include <arenaforge_core/layers/ShapeLayer.h>

namespace arenaforge {
std::shared_ptr<ShapeLayer> ShapeLayer::Make(const std::string &layerId) {
    return std::shared_ptr<ShapeLayer>(new ShapeLayer(layerId));
}

ShapeLayer::ShapeLayer(const std::string &layerId)
    : Layer(layerId)
    , _fill(std::nullopt)
    , _stroke(std::nullopt) {
}

void ShapeLayer::setLineWidth(float width) {
    _lineWidth = width;
}

void ShapeLayer::setStrokeAlign(StrokeAlign align) {
    _strokeAlign = align;
}

void ShapeLayer::setLineCap(LineCap cap) {
    _lineCap = cap;
}

void ShapeLayer::setLineJoin(LineJoin join) {
    _lineJoin = join;
}

void ShapeLayer::setFill(std::optional<Color> fill) {
    _fill = std::move(fill);
}

void ShapeLayer::setStroke(std::optional<Color> stroke) {
    _stroke = std::move(stroke);
}

void ShapeLayer::setPathCommands(std::vector<PathCommand> commands) {
    _pathCommands = std::move(commands);
}
};  // namespace arenaforge
