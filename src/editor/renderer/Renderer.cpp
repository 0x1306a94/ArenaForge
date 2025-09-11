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
//  Renderer.cpp
//  arenaforge
//
//  Created by king on 2025/9/8.
//

#include "Renderer.h"

#include "RendererBackend.h"
#include "RendererState.h"

#include "drawers/GridBackgroundLayerTree.h"
#include "drawers/UserDesignLayerTree.h"

#include <tgfx/core/Canvas.h>
#include <tgfx/core/Surface.h>
#include <tgfx/gpu/Device.h>
#include <tgfx/gpu/Window.h>
#include <tgfx/platform/Print.h>

namespace arenaforge::editor {
std::shared_ptr<Renderer> Renderer::Make(std::shared_ptr<RendererState> state, std::shared_ptr<RendererBackend> backend) {
    return std::shared_ptr<Renderer>(new Renderer(std::move(state), std::move(backend)));
}

Renderer::Renderer(std::shared_ptr<RendererState> state, std::shared_ptr<RendererBackend> backend)
    : _state(std::move(state))
    , _backend(std::move(backend))
    , _gridLayer(std::make_unique<GridBackgroundLayerTree>())
    , _designLayerTree(std::make_unique<UserDesignLayerTree>())
    , _invalidate(false) {
}

Renderer::~Renderer() {
    purgeResources();
    tgfx::PrintLog("%s", __PRETTY_FUNCTION__);
}

std::shared_ptr<RendererBackend> Renderer::getRendererBackend() {
    return _backend;
}

void Renderer::replaceBackend(std::shared_ptr<RendererBackend> backend) {
    _backend = std::move(backend);
}

bool Renderer::updateSize() {
    if (_backend == nullptr || _state == nullptr) {
        return false;
    }

    auto window = _backend->getWindow();
    if (window == nullptr) {
        return false;
    }

    auto width = _backend->getWidth();
    auto height = _backend->getHeight();
    auto density = _backend->getDensity();
    auto sizeChanged = _state->updateBounds(tgfx::Size{static_cast<float>(width), static_cast<float>(height)}, density);
    if (sizeChanged) {
        window->invalidSize();
    }
    return sizeChanged;
}

void Renderer::invalidateContent() {
    _invalidate = true;
}

tgfx::Layer *Renderer::designLayerRoot() const {
    return _designLayerTree->root();
}

std::vector<std::shared_ptr<tgfx::Layer>> Renderer::getDesignLayersUnderPoint(float x, float y) const {
    return _designLayerTree->getLayersUnderPoint(x, y);
}

void Renderer::draw(bool force) {
    if (_backend == nullptr) {
        return;
    }

    if (_state == nullptr) {
        return;
    }

    auto window = _backend->getWindow();
    if (window == nullptr) {
        return;
    }

    auto device = window->getDevice();
    if (device == nullptr) {
        return;
    }

    auto context = device->lockContext();
    if (context == nullptr) {
        return;
    }

    auto surface = window->getSurface(context);
    if (surface == nullptr) {
        device->unlock();
        return;
    }

    auto canvas = surface->getCanvas();
    if (canvas == nullptr) {
        device->unlock();
        return;
    }

    auto statePtr = _state.get();
//    _gridLayer->prepare(canvas, statePtr, force);
    _designLayerTree->prepare(canvas, statePtr, force);

    bool hasContentChanged = /*_gridLayer->hasContentChanged() ||*/ _designLayerTree->hasContentChanged();

    if (!hasContentChanged && !force && !_invalidate) {
        device->unlock();
        return;
    }

    canvas->clear();
    canvas->save();

//    _gridLayer->draw(canvas, statePtr);
    _designLayerTree->draw(canvas, statePtr);

    canvas->restore();

    context->flushAndSubmit();
    window->present(context);

    device->unlock();

    _invalidate = false;
}

void Renderer::purgeResources() {
    if (_backend == nullptr) {
        return;
    }

    auto window = _backend->getWindow();
    if (window == nullptr) {
        return;
    }

    auto device = window->getDevice();
    if (device == nullptr) {
        return;
    }

    auto context = device->lockContext();
    if (context == nullptr) {
        return;
    }
    context->purgeResourcesUntilMemoryTo(0);
    device->unlock();
}
};  // namespace arenaforge::editor
