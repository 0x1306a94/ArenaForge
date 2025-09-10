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

#include <arenaforge_core/PathCommand.h>
#include <arenaforge_core/layers/ShapeType.h>

#include <tgfx/layers/ShapeLayer.h>

#include <string>
#include <unordered_map>

namespace arenaforge {
class BaseLayer : public tgfx::ShapeLayer {
  public:
    static std::shared_ptr<BaseLayer> Make(const std::string &layerId, ShapeType type);

    virtual ~BaseLayer();

    std::string layerId() const {
        return _layerId;
    }

    void setLayerId(const std::string &layerId);

    bool locked() const {
        return _locked;
    }

    void setLocked(bool value);

    bool transient() const {
        return _transient;
    }

    void setTransient(bool value);

    bool positionRelative() const {
        return _positionRelative;
    }

    void setPositionRelative(bool value);

    tgfx::Rect frame() const {
        return _frame;
    }

    void setFrame(const tgfx::Rect &frame);

    const std::unordered_map<std::string, std::string> attributes() const {
        return _attributes;
    }

    void clearAttributes();

    void addAttribute(const std::string &key, const std::string &value);

    const std::vector<PathCommand> pathCommands() const {
        return _pathCommands;
    }

    void setPathCommands(const std::vector<PathCommand> &commands);

    std::shared_ptr<BaseLayer> getChildById(const std::string &layerId);

    virtual std::shared_ptr<BaseLayer> clone(bool cloneChildren) const;

    std::string toJSON(bool pretty = false);

  protected:
    BaseLayer(const std::string &layerId, ShapeType type);

    virtual void doClone(BaseLayer *target, bool cloneChildren) const;

    void updatePathCommands();

    void onUpdateContent(tgfx::LayerRecorder *recorder) override;

  private:
    void initializePathCommand(ShapeType type);

  private:
    std::string _layerId{""};
    bool _transient{false};
    bool _locked{false};
    bool _positionRelative{false};
    std::unordered_map<std::string, std::string> _attributes{};
    tgfx::Rect _frame{};
    std::vector<PathCommand> _pathCommands{};
};
};  // namespace arenaforge

#endif /* BaseLayer_h_ArenaForge */
