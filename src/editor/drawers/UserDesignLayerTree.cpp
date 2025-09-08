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
//  UserDesignLayerTree.cpp
//  arenaforge
//
//  Created by king on 2025/9/8.
//

#include "UserDesignLayerTree.h"

#include "renderer/RendererState.h"

#include <arenaforge/editor/core/defines.h>

#include <tgfx/layers/DisplayList.h>
#include <tgfx/layers/Layer.h>
#include <tgfx/layers/ShapeLayer.h>
#include <tgfx/layers/SolidColor.h>
#include <tgfx/platform/Print.h>

namespace arenaforge::editor {
UserDesignLayerTree::UserDesignLayerTree()
    : Drawer("UserDesignLayerTree")
    , _displayList(std::make_unique<tgfx::DisplayList>()) {
    tgfx::PrintLog("%s", __PRETTY_FUNCTION__);

    _displayList->setRenderMode(tgfx::RenderMode::Tiled);
    _displayList->setAllowZoomBlur(true);
    _displayList->setMaxTileCount(512);
}

UserDesignLayerTree::~UserDesignLayerTree() {
    tgfx::PrintLog("%s", __PRETTY_FUNCTION__);
}

std::vector<std::shared_ptr<tgfx::Layer>> UserDesignLayerTree::getLayersUnderPoint(float x, float y) {
    return _displayList->root()->getLayersUnderPoint(x, y);
}

bool UserDesignLayerTree::hasContentChanged() const {
    if (_root == nullptr) {
        return true;
    }
    return _displayList->hasContentChanged();
}

tgfx::Layer *UserDesignLayerTree::root() const {
    return _displayList->root();
}

void UserDesignLayerTree::prepare(tgfx::Canvas *canvas, const RendererState *state, bool force) {
    UNUSED_PARAM(canvas);
    UNUSED_PARAM(force);

    auto zoomScale = state->zoomScale();
    auto contentOffset = state->contentOffset();
    _displayList->setZoomScale(zoomScale);
    _displayList->setContentOffset(contentOffset.x, contentOffset.y);
}

void UserDesignLayerTree::onDraw(tgfx::Canvas *canvas, const RendererState *state) {
    UNUSED_PARAM(state);
    auto surface = canvas->getSurface();
    _displayList->render(surface, false);
}

};  // namespace arenaforge::editor
