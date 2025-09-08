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
//  Drawer.h
//  arenaforge
//
//  Created by king on 2025/9/8.
//

#ifndef Drawer_h_ArenaForge
#define Drawer_h_ArenaForge

#include <string>

namespace tgfx {
class Canvas;
class Layer;
class DisplayList;
};  // namespace tgfx

namespace arenaforge::editor {
class RendererState;
class Drawer {
  public:
    explicit Drawer(const std::string &name);
    virtual ~Drawer() = default;
    std::string name() const {
        return _name;
    }

    virtual void prepare(tgfx::Canvas *canvas, const RendererState *state, bool force = false) = 0;
    void draw(tgfx::Canvas *canvas, const RendererState *state);

  protected:
    virtual void onDraw(tgfx::Canvas *canvas, const RendererState *state) = 0;

  private:
    std::string _name;
};
};  // namespace arenaforge::editor

#endif /* Drawer_h_ArenaForge */
