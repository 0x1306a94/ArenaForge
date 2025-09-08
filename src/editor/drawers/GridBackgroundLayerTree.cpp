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
//  GridBackgroundLayerTree.cpp
//  arenaforge
//
//  Created by king on 2025/9/8.
//

#include "GridBackgroundLayerTree.h"

#include "renderer/RendererState.h"

#include <arenaforge_editor/core/defines.h>

#include <tgfx/core/Canvas.h>
#include <tgfx/layers/DisplayList.h>
#include <tgfx/layers/LayerRecorder.h>
#include <tgfx/platform/Print.h>

namespace arenaforge::editor {
class GridBackgroundLayer : public tgfx::Layer {
  private:
    tgfx::Size _contentSize{};
    float _density{1.0};

  public:
    static std::shared_ptr<GridBackgroundLayer> Make() {
        return std::shared_ptr<GridBackgroundLayer>(new GridBackgroundLayer());
    }

    void setContentSize(const tgfx::Size &contentSize) {
        if (_contentSize == contentSize) {
            return;
        }
        _contentSize = contentSize;
        invalidateContent();
    }

    void setDensity(float density) {
        if (_density == density) {
            return;
        }
        _density = density;
        invalidateContent();
    }

  protected:
    GridBackgroundLayer() = default;
    void onUpdateContent(tgfx::LayerRecorder *recorder) override {
        if (_contentSize.isEmpty()) {
            return;
        }
        auto canvas = recorder->getCanvas();
        auto width = static_cast<int>(_contentSize.width);
        auto height = static_cast<int>(_contentSize.height);
        canvas->clear(tgfx::Color::White());
        tgfx::Paint paint = {};
        paint.setColor(tgfx::Color{0.8f, 0.8f, 0.8f, 1.f});
        int tileSize = 8 * static_cast<int>(_density);
        for (int y = 0; y < height; y += tileSize) {
            bool draw = (y / tileSize) % 2 == 1;
            for (int x = 0; x < width; x += tileSize) {
                if (draw) {
                    auto rect = tgfx::Rect::MakeXYWH(static_cast<float>(x), static_cast<float>(y), static_cast<float>(tileSize), static_cast<float>(tileSize));
                    canvas->drawRect(rect, paint);
                }
                draw = !draw;
            }
        }
    }
};

GridBackgroundLayerTree::GridBackgroundLayerTree()
    : Drawer("GridBackgroundLayerTree")
    , _root(nullptr)
    , _displayList(std::make_unique<tgfx::DisplayList>()) {
    tgfx::PrintLog("%s", __PRETTY_FUNCTION__);
}

GridBackgroundLayerTree::~GridBackgroundLayerTree() {
    tgfx::PrintLog("%s", __PRETTY_FUNCTION__);
}

bool GridBackgroundLayerTree::hasContentChanged() const {
    if (_root == nullptr) {
        return true;
    }
    return _displayList->hasContentChanged();
}

void GridBackgroundLayerTree::prepare(tgfx::Canvas *canvas, const RendererState *state, bool force) {
    UNUSED_PARAM(canvas);
    UNUSED_PARAM(force);
    if (_root != nullptr) {
        _root->setContentSize(state->getBoundsSize());
        _root->setDensity(state->density());
    }
}

void GridBackgroundLayerTree::onDraw(tgfx::Canvas *canvas, const RendererState *state) {
    UNUSED_PARAM(state);
    if (_root == nullptr) {
        _root = GridBackgroundLayer::Make();
        _root->setShouldRasterize(true);
        _displayList->root()->addChild(_root);
        _displayList->setRenderMode(tgfx::RenderMode::Direct);
    }

    auto surface = canvas->getSurface();
    _displayList->render(surface, false);
}
};  // namespace arenaforge::editor
