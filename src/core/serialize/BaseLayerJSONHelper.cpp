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
//  BaseLayerJSONHelper.cpp
//  arenaforge_core
//
//  Created by king on 2025/9/10.
//

#include "BaseLayerJSONHelper.h"

#include "JSONSerializeHelper.h"

#include <arenaforge_core/PathCommand.h>

#include <tgfx/layers/SolidColor.h>

namespace nlohmann {

namespace as = arenaforge::json;
std::shared_ptr<arenaforge::BaseLayer> adl_serializer<std::shared_ptr<arenaforge::BaseLayer>>::from_json(const nlohmann::json &j) {
    auto layerId = as::read_string_value(j, "layerId", "");
    auto layer = arenaforge::BaseLayer::Make(layerId, arenaforge::ShapeType::Rectangle);
    layer->setName(as::read_string_value(j, "name", ""));
    layer->setFrame(as::read_value(j, "frame", tgfx::Rect::MakeEmpty()));
    layer->setVisible(as::read_value(j, "visible", true));
    layer->setLocked(as::read_value(j, "locked", false));
    layer->setAlpha(as::read_value(j, "alpha", 1.0f));
    layer->setPositionRelative(as::read_value(j, "positionRelative", false));

    do {
        auto matrixIt = j.find("matrix");
        if (matrixIt == j.end()) {
            break;
        }
        auto buffer = matrixIt->get<std::array<float, 9>>();
        auto matrix = tgfx::Matrix::I();
        matrix.setAll(buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
        layer->setMatrix(matrix);
    } while (0);

    do {
        auto pathsIt = j.find("paths");
        if (pathsIt == j.end()) {
            break;
        }
        auto paths = pathsIt->get<std::vector<arenaforge::PathCommand>>();
        layer->setPathCommands(paths);
    } while (0);

    do {
        auto styleIt = j.find("style");
        if (styleIt == j.end()) {
            break;
        }
        auto &style = *styleIt;
        layer->setLineCap(static_cast<tgfx::LineCap>(style["lineCap"].get<int>()));
        layer->setLineJoin(static_cast<tgfx::LineJoin>(style["lineJoin"].get<int>()));
        layer->setLineWidth(style["lineCap"].get<float>());
        layer->setMiterLimit(style["miterLimit"].get<float>());
        layer->setLineDashPattern(style["lineDashPattern"].get<std::vector<float>>());
        layer->setLineDashPhase(style["lineDashPhase"].get<float>());
        layer->setLineDashAdaptive(style["lineDashAdaptive"].get<bool>());
        layer->setStrokeStart(style["strokeStart"].get<float>());
        layer->setStrokeEnd(style["strokeEnd"].get<float>());
        layer->setStrokeAlign(static_cast<tgfx::StrokeAlign>(style["strokeAlign"].get<int>()));
        layer->setStrokeOnTop(style["strokeOnTop"].get<bool>());

        auto fillIt = style.find("fills");
        do {
            if (fillIt == style.end()) {
                break;
            }
            std::vector<std::shared_ptr<tgfx::ShapeStyle>> fillStyles;
            for (auto item : *fillIt) {
                auto style = item.get<std::shared_ptr<tgfx::SolidColor>>();
                fillStyles.push_back(style);
            }

            layer->setFillStyles(std::move(fillStyles));
        } while (0);

        auto strokeIt = style.find("strokes");
        do {
            if (strokeIt == style.end()) {
                break;
            }
            std::vector<std::shared_ptr<tgfx::ShapeStyle>> strokeStyles;
            for (auto item : *strokeIt) {
                auto style = item.get<std::shared_ptr<tgfx::SolidColor>>();
                strokeStyles.push_back(style);
            }

            layer->setStrokeStyles(std::move(strokeStyles));
        } while (0);
    } while (0);
    return layer;
}

void adl_serializer<std::shared_ptr<arenaforge::BaseLayer>>::to_json(nlohmann::json &j, std::shared_ptr<arenaforge::BaseLayer> layer) {
    j["layerId"] = layer->layerId();
    j["name"] = layer->name();
    j["frame"] = layer->frame();
    j["visible"] = layer->visible();
    j["locked"] = layer->locked();
    j["alpha"] = layer->alpha();
    j["positionRelative"] = layer->positionRelative();

    auto matrix = layer->matrix();
    if (!matrix.isIdentity()) {
        std::array<float, 9> buffer;
        matrix.get9(buffer.data());
        j["matrix"] = buffer;
    }

    do {
        auto commands = layer->pathCommands();
        auto paths = nlohmann::json::array();
        for (const auto &cmd : commands) {
            paths.push_back(cmd);
        }
        j["paths"] = paths;
    } while (0);

    do {
        auto style = nlohmann::json::object();

        auto fillStyles = nlohmann::json::array();
        for (auto it : layer->fillStyles()) {
            auto style = std::static_pointer_cast<tgfx::SolidColor>(it);
            fillStyles.push_back(style);
        }
        style["fills"] = fillStyles;

        auto strokeStyles = nlohmann::json::array();
        for (auto it : layer->strokeStyles()) {
            auto style = std::static_pointer_cast<tgfx::SolidColor>(it);
            strokeStyles.push_back(style);
        }
        style["strokes"] = strokeStyles;

        style["lineCap"] = static_cast<int>(layer->lineCap());
        style["lineJoin"] = static_cast<int>(layer->lineJoin());
        style["lineWidth"] = layer->lineWidth();
        style["miterLimit"] = layer->miterLimit();
        style["lineDashPattern"] = layer->lineDashPattern();
        style["lineDashPhase"] = layer->lineDashPhase();
        style["lineDashAdaptive"] = layer->lineDashAdaptive();
        style["strokeStart"] = layer->strokeStart();
        style["strokeEnd"] = layer->strokeEnd();
        style["strokeAlign"] = static_cast<int>(layer->strokeAlign());
        style["strokeOnTop"] = layer->strokeOnTop();

        j["style"] = style;
    } while (0);

    auto children = nlohmann::json::array();
    for (auto it : layer->children()) {
        auto baseLayer = std::static_pointer_cast<arenaforge::BaseLayer>(it);
        if (baseLayer->transient()) {
            continue;
        }
        children.push_back(baseLayer);
    }

    j["childrens"] = children;
}

arenaforge::PathCommand adl_serializer<arenaforge::PathCommand>::from_json(const nlohmann::json &j) {
    auto type = arenaforge::PathCommandTypeFromString(as::read_string_value(j, "type", ""));
    arenaforge::PathCommand cmd;
    cmd.type = type;
    do {
        if (cmd.type == arenaforge::PathCommandType::ClosePath || cmd.type == arenaforge::PathCommandType::Unknown) {
            break;
        }
        as::read_field(cmd.c1, j, "c1");
        as::read_field(cmd.c2, j, "c2");
        as::read_field(cmd.p, j, "p");
    } while (0);
    return cmd;
}

void adl_serializer<arenaforge::PathCommand>::to_json(nlohmann::json &j, const arenaforge::PathCommand &cmd) {
    j["type"] = arenaforge::PathCommandTypeToString(cmd.type);
    if (cmd.type == arenaforge::PathCommandType::ClosePath || cmd.type == arenaforge::PathCommandType::Unknown) {
        return;
    }

    j["c1"] = cmd.c1;
    j["c2"] = cmd.c2;
    j["p"] = cmd.p;
}

std::shared_ptr<tgfx::SolidColor> adl_serializer<std::shared_ptr<tgfx::SolidColor>>::from_json(const nlohmann::json &j) {
    auto style = tgfx::SolidColor::Make();
    style->setBlendMode(static_cast<tgfx::BlendMode>(as::read_value<int>(j, "blendMode")));
    auto color = j["color"].get<tgfx::Color>();
    style->setColor(color);
    return style;
}

void adl_serializer<std::shared_ptr<tgfx::SolidColor>>::to_json(nlohmann::json &j, std::shared_ptr<tgfx::SolidColor> style) {
    j["blendMode"] = static_cast<int>(style->blendMode());
    j["alpha"] = style->alpha();
    j["color"] = style->color();
}

tgfx::Color adl_serializer<tgfx::Color>::from_json(const nlohmann::json &j) {
    auto r = as::read_value<uint8_t>(j, "r", 0);
    auto g = as::read_value<uint8_t>(j, "g", 0);
    auto b = as::read_value<uint8_t>(j, "b", 0);
    auto a = as::read_value<uint8_t>(j, "a", 0);

    return tgfx::Color::FromRGBA(r, g, b, a);
}

void adl_serializer<tgfx::Color>::to_json(nlohmann::json &j, const tgfx::Color &color) {
    j["r"] = static_cast<uint8_t>(color.red * 255.0f);
    j["g"] = static_cast<uint8_t>(color.green * 255.0f);
    j["b"] = static_cast<uint8_t>(color.blue * 255.0f);
    j["a"] = static_cast<uint8_t>(color.alpha * 255.0f);
}

tgfx::Rect adl_serializer<tgfx::Rect>::from_json(const nlohmann::json &j) {
    auto x = as::read_value<float>(j, "x", 0.0f);
    auto y = as::read_value<float>(j, "y", 0.0f);
    auto w = as::read_value<float>(j, "w", 0.0f);
    auto h = as::read_value<float>(j, "h", 0.0f);
    return tgfx::Rect::MakeXYWH(x, y, w, h);
}

void adl_serializer<tgfx::Rect>::to_json(nlohmann::json &j, const tgfx::Rect &rect) {
    j["x"] = rect.x();
    j["y"] = rect.y();
    j["w"] = rect.width();
    j["h"] = rect.height();
}

tgfx::Point adl_serializer<tgfx::Point>::from_json(const nlohmann::json &j) {
    auto x = as::read_value<float>(j, "x", 0.0f);
    auto y = as::read_value<float>(j, "y", 0.0f);
    return tgfx::Point::Make(x, y);
}

void adl_serializer<tgfx::Point>::to_json(nlohmann::json &j, const tgfx::Point &point) {
    j["x"] = point.x;
    j["y"] = point.y;
}

}  // namespace nlohmann
