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
//  Drawer.cpp
//  arenaforge
//
//  Created by king on 2025/9/8.
//

#include "Drawer.h"

#include "renderer/RendererState.h"

#include <tgfx/core/Canvas.h>
#include <tgfx/platform/Print.h>

namespace arenaforge::editor {
Drawer::Drawer(const std::string &name)
    : _name(name) {
}

void Drawer::draw(tgfx::Canvas *canvas, const RendererState *state) {
    if (canvas == nullptr) {
        tgfx::PrintError("Drawer::draw() canvas is nullptr!");
        return;
    }
    if (state == nullptr) {
        tgfx::PrintError("Drawer::draw() state is nullptr!");
        return;
    }
    tgfx::AutoCanvasRestore autoRestore(canvas);
    onDraw(canvas, state);
}
};  // namespace arenaforge::editor
