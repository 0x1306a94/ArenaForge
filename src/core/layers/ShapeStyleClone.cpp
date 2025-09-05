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
//  ShapeStyleClone.cpp
//  arenaforge_core
//
//  Created by king on 2025/9/5.
//

#include "ShapeStyleClone.h"

#include <tgfx/layers/SolidColor.h>

namespace arenaforge {
void cloneShapeStyleBase(const tgfx::ShapeStyle *source, tgfx::ShapeStyle *target) {
    if (source == nullptr || target == nullptr) {
        return;
    }
    target->setBlendMode(source->blendMode());
    target->setAlpha(source->alpha());
    target->setMatrix(source->matrix());
}

std::shared_ptr<tgfx::ShapeStyle> cloneShapeStyle(const tgfx::ShapeStyle *style) {
    if (style == nullptr) {
        return nullptr;
    }

    if (auto source = dynamic_cast<const tgfx::SolidColor *>(style)) {
        auto copied = tgfx::SolidColor::Make(source->color());
        cloneShapeStyleBase(source, copied.get());
        return copied;
    }

    return nullptr;
}
};  // namespace arenaforge
