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
//  ShapeLayerBridge.h
//  arenaforge_editor
//
//  Created by king on 2025/9/17.
//

#ifndef ShapeLayerBridge_h_ArenaForge
#define ShapeLayerBridge_h_ArenaForge

#include "LayerBridge.h"

namespace tgfx {
class ShapeLayer;
};

namespace arenaforge {
class ShapeLayer;
class ShapeLayerBridge : public LayerBridge {
  public:
    static std::shared_ptr<ShapeLayerBridge> Make(tgfx::Layer *renderParent, std::shared_ptr<ShapeLayer> data);
    virtual ~ShapeLayerBridge() = default;

    std::shared_ptr<Layer> dataLayer() override;

    std::shared_ptr<tgfx::Layer> renderLayer() override;

    void buildRenderLayer() override;

  protected:
    ShapeLayerBridge(tgfx::Layer *renderParent, std::shared_ptr<ShapeLayer> data);

  private:
    std::shared_ptr<ShapeLayer> _data;
    std::shared_ptr<tgfx::ShapeLayer> _renderLayer;
};
};  // namespace arenaforge

#endif /* ShapeLayerBridge_h_ArenaForge */
