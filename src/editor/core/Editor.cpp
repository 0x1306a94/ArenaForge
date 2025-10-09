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

#include <arenaforge_editor/core/HoverManager.h>
#include <arenaforge_editor/core/SelectionManager.h>

#include <arenaforge_core/Point.h>
#include <arenaforge_core/Print.h>
#include <arenaforge_core/Project.h>
#include <arenaforge_core/Size.h>
#include <arenaforge_core/layers/Layer.h>
#include <arenaforge_core/layers/ShapeLayer.h>

#include "renderer/LayerTreeAdapter.h"
#include "renderer/PathBuilder.h"
#include "renderer/Renderer.h"
#include "renderer/RendererBackend.h"
#include "renderer/RendererState.h"

#include <tgfx/layers/Layer.h>
#include <tgfx/layers/ShapeLayer.h>
#include <tgfx/layers/SolidColor.h>
#include <tgfx/layers/TextLayer.h>

namespace arenaforge::editor {

std::shared_ptr<Editor> Editor::Make(std::shared_ptr<arenaforge::Project> project) {
    return std::shared_ptr<Editor>(new Editor(std::move(project)));
}

Editor::Editor(std::shared_ptr<arenaforge::Project> project)
    : _project(std::move(project))
    , _renderer(Renderer::Make(std::make_shared<RendererState>(), nullptr)) {

    setupRootLayer();
    setupProjectSynchronization();

    auto root = _project->root();
    _treeAdapter = std::make_unique<LayerTreeAdapter>(root, _containerLayer);

    _selectionManager = SelectionManager::Make(_treeAdapter.get(), _renderer->getOverlayRootLayer());
    _hoverManager = HoverManager::Make(_treeAdapter.get(), _renderer->getOverlayRootLayer(), _rootLayer);
}

Editor::~Editor() {
    PrintLog("%s", __PRETTY_FUNCTION__);
}

void Editor::setRendererBackend(std::shared_ptr<RendererBackend> rendererBackend) {
    if (_renderer) {
        _renderer->replaceBackend(std::move(rendererBackend));
    }
}

arenaforge::Size Editor::getBoundsSize() const {
    if (!_renderer) {
        return arenaforge::Size::MakeEmpty();
    }

    auto state = _renderer->state();
    if (!state) {
        return arenaforge::Size::MakeEmpty();
    }
    auto bounds = state->getBoundsSize();
    return bounds;
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
arenaforge::Point Editor::contentOffset() const {
    if (!_renderer) {
        return arenaforge::Point::Zero();
    }

    auto state = _renderer->state();
    if (!state) {
        return arenaforge::Point::Zero();
    }
    auto contentOffset = state->contentOffset();
    return contentOffset;
}

bool Editor::updateZoomAndOffset(float zoomScale, float offsetX, float offsetY) {
    if (!_renderer) {
        return false;
    }

    auto state = _renderer->state();
    if (!state) {
        return false;
    }
    auto changed = state->updateZoomAndOffset(zoomScale, Point{offsetX, offsetY});
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

    auto state = _renderer->state();
    if (!state) {
        return;
    }

    auto designLayer = _renderer->getDesignRootLayer();
    if (!designLayer) {
        return;
    }

    auto viewSize = state->getBoundsSize();
    auto contentBounds = designLayer->getBounds();
    auto contentWidth = contentBounds.width();
    auto contentHeight = contentBounds.height();

    float padding = 100.0f * state->density();

    // 如果内容加上 padding 后仍然小于视口，就不缩放
    if (contentWidth + 2 * padding < viewSize.width &&
        contentHeight + 2 * padding < viewSize.height) {

        float offsetX = (viewSize.width - contentWidth) * 0.5f;
        float offsetY = (viewSize.height - contentHeight) * 0.5f;

        state->updateZoomAndOffset(1.0f, Point{offsetX, offsetY});
        return;
    }

    // 缩放比例：在宽和高方向都考虑 padding
    float scaleX = viewSize.width / (contentWidth + 2 * padding);
    float scaleY = viewSize.height / (contentHeight + 2 * padding);
    float scale = std::min(scaleX, scaleY);

    // offset 需要考虑 bounds 的 left/top 和 padding
    float offsetX = (viewSize.width - (contentWidth + 2 * padding) * scale) / 2.0f - (contentBounds.left - padding) * scale;
    float offsetY = (viewSize.height - (contentHeight + 2 * padding) * scale) / 2.0f - (contentBounds.top - padding) * scale;

    state->updateZoomAndOffset(scale, Point{offsetX, offsetY});
}

bool Editor::hitTestPointInContainer(float x, float y, bool shapeHitTest) const {
    return _containerLayer->hitTestPoint(x, y, shapeHitTest);
}

std::shared_ptr<arenaforge::Layer> Editor::findLayerAtPoint(float x, float y, bool shapeHitTest) const {
    auto layers = _containerLayer->getLayersUnderPoint(x, y);
    if (layers.empty()) {
        return nullptr;
    }

    auto hitLayer = layers.front();
    if (hitLayer == _containerLayer) {
        return nullptr;
    }

    if (shapeHitTest) {
        for (auto &layer : layers) {
            if (layer->hitTestPoint(x, y, true)) {
                hitLayer = layer;
                break;
            }
        }
    }

    if (!hitLayer) {
        return nullptr;
    }

    auto dataLayer = _treeAdapter->findDataLayer(hitLayer->name());
    return dataLayer;
}

std::shared_ptr<arenaforge::Layer> Editor::getLayerByLayerId(const std::string &layerId) const {
    auto layer = _treeAdapter->findDataLayer(layerId);
    return layer;
}

std::optional<arenaforge::Point> Editor::globalToLocal(const arenaforge::Point &global, const std::shared_ptr<arenaforge::Layer> &sourceLayer) const {
    if (!sourceLayer) {
        return std::nullopt;
    }
    auto renderLayer = sourceLayer->isRoot() ? _rootLayer : _treeAdapter->findRenderLayer(sourceLayer->layerId());
    if (!renderLayer) {
        return std::nullopt;
    }
    auto local = renderLayer->globalToLocal({global.x, global.y});
    return arenaforge::Point{local.x, local.y};
}

std::optional<arenaforge::Point> Editor::localToGlobal(const arenaforge::Point &local, const std::shared_ptr<arenaforge::Layer> &sourceLayer) const {
    if (!sourceLayer) {
        return std::nullopt;
    }
    auto renderLayer = sourceLayer->isRoot() ? _rootLayer : _treeAdapter->findRenderLayer(sourceLayer->layerId());
    if (!renderLayer) {
        return std::nullopt;
    }
    auto global = renderLayer->localToGlobal({local.x, local.y});
    return arenaforge::Point{global.x, global.y};
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
    auto root = _project->root();

    auto frame = root->frame();

    tgfx::Path rootPath;
    rootPath.addRect(tgfx::Rect::MakeWH(frame.width(), frame.height()));

    auto layerTreeRoot = _renderer->getDesignRootLayer();
    _rootLayer = tgfx::ShapeLayer::Make();
    _rootLayer->setPath(rootPath);

    _containerLayer = tgfx::ShapeLayer::Make();
    _containerLayer->setFillStyle(tgfx::SolidColor::Make(tgfx::Color::FromRGBA(0xcc, 0xcc, 0xcc)));
    _containerLayer->setPath(rootPath);

    _maskLayer = tgfx::ShapeLayer::Make();
    _maskLayer->setFillStyle(tgfx::SolidColor::Make(tgfx::Color::FromRGBA(0xcc, 0xcc, 0xcc)));
    _maskLayer->setPath(rootPath);

    _rootLayer->addChild(_containerLayer);
    _rootLayer->addChild(_maskLayer);

    _containerLayer->setMask(_maskLayer);

    layerTreeRoot->addChild(_rootLayer);
}

void Editor::setupProjectSynchronization() {
    if (!_project) {
        return;
    }

    _project->setOnCanvasSizeChanged([this](arenaforge::Project *project) {
        UNUSED_PARAM(project);
        this->handleProjectCanvasSizeChanged();
    });
}

void Editor::handleProjectCanvasSizeChanged() {
    if (!_project) {
        return;
    }

    auto root = _project->root();
    auto frame = root->frame();

    tgfx::Path rootPath;
    rootPath.addRect(tgfx::Rect::MakeWH(frame.width(), frame.height()));
    _rootLayer->setPath(rootPath);
    _containerLayer->setPath(rootPath);
    _maskLayer->setPath(rootPath);
}

};  // namespace arenaforge::editor
