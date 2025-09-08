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
//  Renderer.h
//  arenaforge
//
//  Created by king on 2025/9/8.
//

#ifndef Renderer_h_ArenaForge
#define Renderer_h_ArenaForge

#include <memory>

namespace tgfx {
class Layer;
class Window;
};  // namespace tgfx

namespace arenaforge::editor {
class RendererBackend;
class RendererState;
class GridBackgroundLayerTree;
class UserDesignLayerTree;
class Renderer : public std::enable_shared_from_this<Renderer> {
  public:
    static std::shared_ptr<Renderer> Make(std::shared_ptr<RendererState> state, std::shared_ptr<RendererBackend> backend);
    ~Renderer();

    std::shared_ptr<RendererBackend> getRendererBackend();

    void replaceBackend(std::shared_ptr<RendererBackend> backend);

    bool updateSize();

    void invalidateContent();
    tgfx::Layer *designLayerRoot() const;
    std::vector<std::shared_ptr<tgfx::Layer>> getDesignLayersUnderPoint(float x, float y) const;

    void draw(bool force = false);

  protected:
    Renderer(std::shared_ptr<RendererState> state, std::shared_ptr<RendererBackend> backend);

  private:
    std::shared_ptr<RendererState> _state;
    std::shared_ptr<RendererBackend> _backend;
    std::unique_ptr<GridBackgroundLayerTree> _gridLayer;
    std::unique_ptr<UserDesignLayerTree> _designLayerTree;
    bool _invalidate;
};
};  // namespace arenaforge::editor

#endif /* Renderer_h_ArenaForge */
