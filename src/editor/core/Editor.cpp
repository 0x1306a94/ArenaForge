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
//  Editor.cpp
//  arenaforge_editor
//
//  Created by KK on 2025/9/6.
//

#include <arenaforge_editor/core/Editor.h>

#include <arenaforge_core/Project.h>
#include <arenaforge_core/Size.h>
#include <arenaforge_core/Venue.h>
#include <arenaforge_core/layers/BaseLayer.h>

#include "renderer/Renderer.h"
#include "renderer/RendererBackend.h"
#include "renderer/RendererState.h"

#include <tgfx/layers/Layer.h>
#include <tgfx/layers/ShapeLayer.h>
#include <tgfx/layers/SolidColor.h>
#include <tgfx/layers/TextLayer.h>
#include <tgfx/platform/Print.h>

namespace arenaforge::editor {

std::shared_ptr<Editor> Editor::Make(std::shared_ptr<arenaforge::Project> project) {
    return std::shared_ptr<Editor>(new Editor(std::move(project)));
}

Editor::Editor(std::shared_ptr<arenaforge::Project> project)
    : _project(std::move(project))
    , _renderer(Renderer::Make(std::make_shared<RendererState>(), nullptr)) {

    setupRootLayer();
}

Editor::~Editor() {
    tgfx::PrintLog("%s", __PRETTY_FUNCTION__);
}

void Editor::setRendererBackend(std::shared_ptr<RendererBackend> rendererBackend) {
    if (_renderer) {
        _renderer->replaceBackend(std::move(rendererBackend));
    }
}

bool Editor::getBoundsSize(float &width, float &height) const {
    if (!_renderer) {
        return false;
    }

    auto state = _renderer->state();
    if (!state) {
        return false;
    }
    auto bounds = state->getBoundsSize();
    width = bounds.width;
    height = bounds.height;
    return true;
}

float Editor::density() const {
    if (!_renderer) {
        return 1.0;
    }

    auto state = _renderer->state();
    if (!state) {
        return 1.0;
    }
    auto density = state->density();
    return density;
}

/// 当前缩放比例
float Editor::zoomScale() const {
    if (!_renderer) {
        return 1.0;
    }

    auto state = _renderer->state();
    if (!state) {
        return 1.0;
    }
    auto zoomScale = state->zoomScale();
    return zoomScale;
}

/// 当前滑动偏移
bool Editor::contentOffset(float &x, float &y) const {
    if (!_renderer) {
        return false;
    }

    auto state = _renderer->state();
    if (!state) {
        return false;
    }
    auto contentOffset = state->contentOffset();
    x = contentOffset.x;
    y = contentOffset.y;
    return true;
}

bool Editor::updateZoomAndOffset(float zoomScale, float offsetX, float offsetY) {
    if (!_renderer) {
        return false;
    }

    auto state = _renderer->state();
    if (!state) {
        return false;
    }
    auto changed = state->updateZoomAndOffset(zoomScale, tgfx::Point{offsetX, offsetY});
    return changed;
}

bool Editor::updateSize() {
    if (!_renderer) {
        return false;
    }
    auto sizeChanged = _renderer->updateSize();
    if (sizeChanged) {
        invalidateContent();
    }
    return sizeChanged;
}

void Editor::autoAdjustCanvasScaleForContent() {
    if (!_renderer) {
        return;
    }
    _renderer->autoAdjustCanvasScaleForContent();
}

void Editor::invalidateContent() {
    if (!_renderer) {
        return;
    }
    _renderer->invalidateContent();
}

void Editor::draw(bool force) {
    if (!_renderer) {
        return;
    }
    _renderer->draw(force);
}

void Editor::setupRootLayer() {
    auto canvasSize = _project->canvasSize();
    _rootLayer = tgfx::ShapeLayer::Make();

    tgfx::Path rootPath;
    rootPath.addRect(tgfx::Rect::MakeWH(canvasSize.width, canvasSize.height));
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

    auto layerTreeRoot = _renderer->designLayerRoot();
    layerTreeRoot->addChild(_rootLayer);
}

};  // namespace arenaforge::editor
