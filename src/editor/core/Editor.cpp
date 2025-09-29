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

#include <arenaforge_core/Point.h>
#include <arenaforge_core/Print.h>
#include <arenaforge_core/Project.h>
#include <arenaforge_core/Size.h>
#include <arenaforge_core/layers/Layer.h>
#include <arenaforge_core/layers/ShapeLayer.h>

#include "renderer/Renderer.h"
#include "renderer/RendererBackend.h"
#include "renderer/RendererState.h"

#include "drawers/LayerTreeAdapter.h"
#include "drawers/PathBuilder.h"

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
    _treeAdapter = std::make_shared<arenaforge::LayerTreeAdapter>(root, _containerLayer);
}

Editor::~Editor() {
    PrintLog("%s", __PRETTY_FUNCTION__);
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
    _renderer->autoAdjustCanvasScaleForContent();
}

bool Editor::hitTestPointInContainer(float x, float y, bool shapeHitTest) const {
    return _containerLayer->hitTestPoint(x, y, shapeHitTest);
}

std::shared_ptr<tgfx::Layer> Editor::findLayerAtPoint(float x, float y, bool shapeHitTest) const {
    auto layers = _containerLayer->getLayersUnderPoint(x, y);
    if (layers.empty()) {
        return nullptr;
    }

    auto topLayer = layers.front();
    if (topLayer == _containerLayer) {
        return nullptr;
    }

    if (shapeHitTest) {
        for (auto &layer : layers) {
            if (layer->hitTestPoint(x, y, true)) {
                return layer;
            }
        }
        return nullptr;
    }

    return topLayer;
}

std::shared_ptr<tgfx::Layer> Editor::getLayerByLayerId(const std::string &layerId) const {
    auto layer = _treeAdapter->findRenderLayer(layerId);
    return layer;
}

void Editor::addHoverWireframe(std::vector<std::shared_ptr<tgfx::Layer>> targets) {
    if (targets.empty()) {
        return;
    }

    for (auto &target : targets) {
        auto key = reinterpret_cast<uintptr_t>(target.get());
        auto it = _hoverWireframeLayers.find(key);
        if (it != _hoverWireframeLayers.end()) {
            continue;
        }

        auto parent = target->parent();
        if (parent == nullptr) {
            continue;
        }

        auto layerId = target->name();
        auto data = (target == _rootLayer) ? _project->root() : _treeAdapter->findDataLayer(layerId);
        if (!data) {
            continue;
        }

        // 先将自身转为全局，再从全局转为_rootLayer
        auto position = target->localToGlobal(tgfx::Point::Zero());
        position = _rootLayer->globalToLocal(position);

        auto frame = data->frame();

        auto hoverLayer = tgfx::ShapeLayer::Make();

        if (data->type() == arenaforge::LayerType::Shape) {
            auto shapeData = std::static_pointer_cast<arenaforge::ShapeLayer>(data);
            const auto &commands = shapeData->pathCommands();
            tgfx::Path path = PathBuilder::BuildPath(commands, frame.size());
            hoverLayer->setPath(std::move(path));
        } else {

            tgfx::Path path;
            path.addRect(tgfx::Rect::MakeWH(frame.width(), frame.height()));
            hoverLayer->setPath(std::move(path));
        }

        hoverLayer->setStrokeStyle(tgfx::SolidColor::Make(tgfx::Color::FromRGBA(0x0c, 0x8c, 0xe9)));
        hoverLayer->setPosition(position);
        hoverLayer->setLineWidth(2);
        hoverLayer->setStrokeAlign(tgfx::StrokeAlign::Outside);

        // 根节点特殊处理
        if (target == _rootLayer) {
            _rootLayer->parent()->addChild(hoverLayer);
        } else {
            _rootLayer->addChild(hoverLayer);
        }
        _hoverWireframeLayers[key] = hoverLayer;
    }
}

void Editor::resetHoverWireframe() {
    if (_hoverWireframeLayers.empty()) {
        return;
    }
    std::unordered_map<uintptr_t, std::weak_ptr<tgfx::Layer>> exists;
    std::swap(_hoverWireframeLayers, exists);
    for (auto &[key, value] : exists) {
        if (auto layer = value.lock()) {
            layer->removeFromParent();
        }
    }
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

    auto layerTreeRoot = _renderer->designLayerRoot();
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
