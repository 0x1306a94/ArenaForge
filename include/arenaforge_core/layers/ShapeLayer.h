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
//  ShapeLayer.h
//  arenaforge_core
//
//  Created by king on 2025/9/17.
//

#ifndef ShapeLayer_h_ArenaForge
#define ShapeLayer_h_ArenaForge

#include <arenaforge_core/PathCommand.h>
#include <arenaforge_core/layers/Layer.h>

#include <arenaforge_core/Color.h>
#include <arenaforge_core/StrokeTypes.h>

#include <optional>

namespace arenaforge {
class ShapeLayer : public Layer {
  public:
    static std::shared_ptr<ShapeLayer> Make(const std::string &layerId);
    virtual ~ShapeLayer() = default;

    LayerType type() const override {
        return LayerType::Shape;
    }

    float lineWidth() const {
        return _lineWidth;
    }

    void setLineWidth(float width);

    StrokeAlign strokeAlign() const {
        return _strokeAlign;
    }

    void setStrokeAlign(StrokeAlign align);

    LineCap lineCap() const {
        return _lineCap;
    }

    void setLineCap(LineCap cap);

    LineJoin lineJoin() const {
        return _lineJoin;
    }

    void setLineJoin(LineJoin join);

    const std::optional<Color> &fill() const {
        return _fill;
    }

    void setFill(std::optional<Color> fill);

    const std::optional<Color> &stroke() const {
        return _stroke;
    }

    void setStroke(std::optional<Color> stroke);

    const std::vector<PathCommand> &pathCommands() const {
        return _pathCommands;
    }

    void setPathCommands(std::vector<PathCommand> commands);

    bool isLine() const;

  protected:
    ShapeLayer(const std::string &layerId);

  private:
    std::optional<Color> _fill;
    std::optional<Color> _stroke;
    float _lineWidth = 1.0f;
    StrokeAlign _strokeAlign{StrokeAlign::Center};
    LineCap _lineCap{LineCap::Butt};
    LineJoin _lineJoin{LineJoin::Miter};
    std::vector<PathCommand> _pathCommands{};
};
};  // namespace arenaforge

#endif /* ShapeLayer_h_ArenaForge */
