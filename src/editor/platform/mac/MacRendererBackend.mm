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
//  MacRendererBackend.mm
//  arenaforge_editor
//
//  Created by KK on 2025/9/6.
//

#include "MacRendererBackend.h"

#include <tgfx/gpu/opengl/cgl/CGLWindow.h>
#include <tgfx/platform/Print.h>

#include <cmath>

namespace arenaforge::editor {
MacRendererBackend::MacRendererBackend(__kindof NSView *hostView)
    : _hostView(hostView) {
    tgfx::PrintLog("%s", __PRETTY_FUNCTION__);
}

MacRendererBackend::~MacRendererBackend() {
    tgfx::PrintLog("%s", __PRETTY_FUNCTION__);
}

std::shared_ptr<tgfx::Window> MacRendererBackend::getWindow() {
    if (_window == nullptr) {
        _window = tgfx::CGLWindow::MakeFrom(_hostView);
    }
    return _window;
}

int MacRendererBackend::getWidth() {
    auto bounds = _hostView.bounds;
    CGSize size = [_hostView convertSizeToBacking:bounds.size];
    auto width = static_cast<int>(roundf(static_cast<float>(size.width)));
    return width;
}

int MacRendererBackend::getHeight() {
    auto bounds = _hostView.bounds;
    CGSize size = [_hostView convertSizeToBacking:bounds.size];
    auto height = static_cast<int>(roundf(static_cast<float>(size.height)));
    return height;
}

float MacRendererBackend::getDensity() {
    auto bounds = _hostView.bounds;
    CGSize size = [_hostView convertSizeToBacking:bounds.size];
    float contentsScale = static_cast<float>(size.height / bounds.size.height);
    return contentsScale;
}
};  // namespace arenaforge::editor
