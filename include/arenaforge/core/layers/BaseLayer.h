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
//  BaseLayer.h
//  arenaforge_core
//
//  Created by king on 2025/9/5.
//

#ifndef BaseLayer_h_ArenaForge
#define BaseLayer_h_ArenaForge

#include <tgfx/layers/ShapeLayer.h>

namespace arenaforge {
class IdGenerator;

class BaseLayer : public tgfx::ShapeLayer {
  public:
    static std::shared_ptr<BaseLayer> Make(uint32_t layerId);

    virtual ~BaseLayer() = default;

    uint32_t layerId() const {
        return _layerId;
    }

    bool transient() const {
        return _transient;
    }

    void setTransient(bool value);

    std::shared_ptr<BaseLayer> getChildById(uint32_t layerId);

    virtual std::shared_ptr<BaseLayer> clone(IdGenerator *idGen, bool cloneChildren) const;

  protected:
    BaseLayer(uint32_t layerId);

    virtual void doClone(BaseLayer *target, IdGenerator *idGen, bool cloneChildren) const;

  private:
    uint32_t _layerId{0};
    bool _transient;
};
};  // namespace arenaforge

#endif /* BaseLayer_h_ArenaForge */
