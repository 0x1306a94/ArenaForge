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
//  LayerStyleClone.cpp
//  arenaforge_core
//
//  Created by king on 2025/9/5.
//

#include "LayerStyleClone.h"

#include <tgfx/layers/layerstyles/BackgroundBlurStyle.h>
#include <tgfx/layers/layerstyles/DropShadowStyle.h>
#include <tgfx/layers/layerstyles/InnerShadowStyle.h>

namespace arenaforge {

void cloneLayerStyleBase(const tgfx::LayerStyle *source, tgfx::LayerStyle *target) {
    if (source == nullptr || target == nullptr) {
        return;
    }
    target->setBlendMode(source->blendMode());
}

std::shared_ptr<tgfx::LayerStyle> cloneLayerStyle(const tgfx::LayerStyle *style) {
    if (style == nullptr) {
        return nullptr;
    }

    auto type = style->Type();
    switch (type) {
        case tgfx::LayerStyleType::InnerShadow: {
            auto source = static_cast<const tgfx::InnerShadowStyle *>(style);
            auto copied = tgfx::InnerShadowStyle::Make(source->offsetX(), source->offsetY(), source->blurrinessX(), source->blurrinessY(), source->color());
            cloneLayerStyleBase(source, copied.get());
            return copied;
        }
        case tgfx::LayerStyleType::DropShadow: {
            auto source = static_cast<const tgfx::DropShadowStyle *>(style);
            auto copied = tgfx::DropShadowStyle::Make(source->offsetX(), source->offsetY(), source->blurrinessX(), source->blurrinessY(), source->color(), source->showBehindLayer());
            cloneLayerStyleBase(source, copied.get());
            return copied;
        }
        case tgfx::LayerStyleType::BackgroundBlur: {
            auto source = static_cast<const tgfx::BackgroundBlurStyle *>(style);
            auto copied = tgfx::BackgroundBlurStyle::Make(source->blurrinessX(), source->blurrinessY(), source->tileMode());
            cloneLayerStyleBase(source, copied.get());
            return copied;
        }
        default:
            break;
    }

    return nullptr;
}
};  // namespace arenaforge
