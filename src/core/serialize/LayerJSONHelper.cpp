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
//  LayerJSONHelper.cpp
//  arenaforge_core
//
//  Created by king on 2025/9/17.
//

#include "LayerJSONHelper.h"

#include "JSONSerializeHelper.h"

#include <arenaforge_core/Color.h>
#include <arenaforge_core/Point.h>
#include <arenaforge_core/Rect.h>

#include <arenaforge_core/layers/BooleanLayer.h>
#include <arenaforge_core/layers/Layer.h>
#include <arenaforge_core/layers/ShapeLayer.h>

namespace nlohmann {

namespace as = arenaforge::json;

static void fill_from_json(const nlohmann::json &j, arenaforge::Layer *target) {
    if (target == nullptr) {
        return;
    }
    auto name = as::read_string_value(j, "name", "");
    target->setName(name);

    auto frame = as::read_value<arenaforge::Rect>(j, "frame", arenaforge::Rect::MakeEmpty());
    target->setFrame(frame);
    target->setVisible(as::read_value<bool>(j, "visible", true));
    target->setLocked(as::read_value<bool>(j, "locked", false));

    do {
        auto childrenIt = j.find("childrens");
        if (childrenIt == j.end()) {
            break;
        }

        auto children = childrenIt->get<std::vector<std::shared_ptr<arenaforge::Layer>>>();
        if (children.empty()) {
            break;
        }

        for (auto it : children) {
            target->addChild(it);
        }

    } while (0);

    do {
        auto attributesIt = j.find("attributes");
        if (attributesIt == j.end()) {
            break;
        }
        auto attributes = attributesIt->get<std::unordered_map<std::string, std::string>>();
        target->setAttributes(std::move(attributes));
    } while (0);
}

static void fill_from_json(const nlohmann::json &j, arenaforge::ShapeLayer *target) {
    if (target == nullptr) {
        return;
    }
    fill_from_json(j, static_cast<arenaforge::Layer *>(target));

    do {
        auto pathsIt = j.find("paths");
        if (pathsIt == j.end()) {
            break;
        }
        auto paths = pathsIt->get<std::vector<arenaforge::PathCommand>>();
        target->setPathCommands(std::move(paths));
    } while (0);

    do {
        auto styleIt = j.find("style");
        if (styleIt == j.end()) {
            break;
        }

        auto &style = *styleIt;
        auto fill = as::read_string_value(style, "fill", "");
        if (!fill.empty()) {
            auto color = arenaforge::Color::FromHex(fill);
            target->setFill(std::move(color));
        }

        auto stroke = as::read_string_value(style, "stroke", "");
        if (!stroke.empty()) {
            auto color = arenaforge::Color::FromHex(stroke);
            target->setStroke(std::move(color));
        }

        auto lineWidth = as::read_value<float>(style, "lineWidth", 1.0f);
        auto lineCap = as::read_value<int>(style, "lineCap", 0);
        auto lineJoin = as::read_value<int>(style, "lineJoin", 0);
        auto strokeAlign = as::read_value<int>(style, "strokeAlign", 0);

        target->setLineWidth(lineWidth);
        target->setLineCap(static_cast<arenaforge::LineCap>(lineCap));
        target->setLineJoin(static_cast<arenaforge::LineJoin>(lineJoin));
        target->setStrokeAlign(static_cast<arenaforge::StrokeAlign>(strokeAlign));
    } while (0);
}

static void fill_from_json(const nlohmann::json &j, arenaforge::BooleanLayer *target) {
    if (target == nullptr) {
        return;
    }
    fill_from_json(j, static_cast<arenaforge::Layer *>(target));
    auto op = as::read_value<int>(j, "boolean", 0);
    target->setOp(static_cast<arenaforge::BooleanOp>(op));
}

static void fill_to_json(nlohmann::json &j, const arenaforge::Layer *layer) {
    if (layer == nullptr) {
        return;
    }
    j["id"] = layer->layerId();
    j["name"] = layer->name();
    j["type"] = static_cast<int>(layer->type());
    j["frame"] = layer->frame();
    j["visible"] = layer->visible();
    j["locked"] = layer->locked();

    auto children = nlohmann::json::array();
    for (auto it : layer->children()) {
        if (it->transient()) {
            continue;
        }
        children.push_back(it);
    }

    if (!children.empty()) {
        j["childrens"] = children;
    }

    auto attributes = nlohmann::json::object();
    for (const auto &[key, value] : layer->attributes()) {
        attributes[key] = value;
    }

    if (!attributes.empty()) {
        j["attributes"] = attributes;
    }
}

static void fill_to_json(nlohmann::json &j, const arenaforge::ShapeLayer *layer) {
    if (layer == nullptr) {
        return;
    }
    fill_to_json(j, static_cast<const arenaforge::Layer *>(layer));

    do {
        auto commands = layer->pathCommands();
        auto paths = nlohmann::json::array();
        for (const auto &cmd : commands) {
            paths.push_back(cmd);
        }
        j["paths"] = paths;
    } while (0);

    nlohmann::json style;
    auto fill = layer->fill();
    if (fill) {
        auto hex = fill.value().hex();
        style["fill"] = hex;
    }

    auto stroke = layer->stroke();
    if (stroke) {
        auto hex = stroke.value().hex();
        style["stroke"] = hex;
        style["lineWidth"] = layer->lineWidth();
        style["lineCap"] = static_cast<int>(layer->lineCap());
        style["lineJoin"] = static_cast<int>(layer->lineJoin());
        style["strokeAlign"] = static_cast<int>(layer->strokeAlign());
    }

    if (!style.empty()) {
        j["style"] = style;
    }
}

static void fill_to_json(nlohmann::json &j, const arenaforge::BooleanLayer *layer) {
    if (layer == nullptr) {
        return;
    }
    fill_to_json(j, static_cast<const arenaforge::Layer *>(layer));
    j["boolean"] = static_cast<int>(layer->op());
}

std::shared_ptr<arenaforge::Layer> adl_serializer<std::shared_ptr<arenaforge::Layer>>::from_json(const nlohmann::json &j) {
    auto layerId = as::read_string_value(j, "id", "");
    auto layerType = static_cast<arenaforge::LayerType>(as::read_value<int>(j, "type", 0));
    std::shared_ptr<arenaforge::Layer> layer = nullptr;
    switch (layerType) {
        case arenaforge::LayerType::Group: {
            layer = arenaforge::Layer::Make(layerId);
            fill_from_json(j, layer.get());
            break;
        }
        case arenaforge::LayerType::Shape: {
            layer = arenaforge::ShapeLayer::Make(layerId);
            fill_from_json(j, static_cast<arenaforge::ShapeLayer *>(layer.get()));
            break;
        }
        case arenaforge::LayerType::Boolean: {
            layer = arenaforge::BooleanLayer::Make(layerId);
            fill_from_json(j, static_cast<arenaforge::BooleanLayer *>(layer.get()));
            break;
        }
        default:
            return nullptr;
    }

    return layer;
}

void adl_serializer<std::shared_ptr<arenaforge::Layer>>::to_json(nlohmann::json &j, std::shared_ptr<arenaforge::Layer> layer) {
    auto layerType = layer->type();
    switch (layerType) {
        case arenaforge::LayerType::Group: {
            fill_to_json(j, layer.get());
            break;
        }
        case arenaforge::LayerType::Shape: {
            fill_to_json(j, static_cast<const arenaforge::ShapeLayer *>(layer.get()));
            break;
        }
        case arenaforge::LayerType::Boolean: {
            fill_to_json(j, static_cast<const arenaforge::BooleanLayer *>(layer.get()));
            break;
        }
        default:
            break;
    }
}

arenaforge::PathCommand adl_serializer<arenaforge::PathCommand>::from_json(const nlohmann::json &j) {
    auto type = arenaforge::PathCommandTypeFromString(as::read_string_value(j, "type", ""));
    arenaforge::PathCommand cmd;
    cmd.type = type;
    do {
        if (cmd.type == arenaforge::PathCommandType::ClosePath || cmd.type == arenaforge::PathCommandType::Unknown) {
            break;
        }

        as::read_field(cmd.p, j, "p");
        if (cmd.type == arenaforge::PathCommandType::QuadTo) {
            as::read_field(cmd.c1, j, "c1");
        } else if (cmd.type == arenaforge::PathCommandType::CubicTo) {
            as::read_field(cmd.c1, j, "c1");
            as::read_field(cmd.c2, j, "c2");
        }
    } while (0);

    return cmd;
}

void adl_serializer<arenaforge::PathCommand>::to_json(nlohmann::json &j, const arenaforge::PathCommand &cmd) {
    j["type"] = arenaforge::PathCommandTypeToString(cmd.type);
    if (cmd.type == arenaforge::PathCommandType::ClosePath || cmd.type == arenaforge::PathCommandType::Unknown) {
        return;
    }

    switch (cmd.type) {
        case arenaforge::PathCommandType::LineTo: {
            j["p"] = cmd.p;
            break;
        }
        case arenaforge::PathCommandType::MoveTo: {
            j["p"] = cmd.p;
            break;
        }
        case arenaforge::PathCommandType::QuadTo: {
            j["p"] = cmd.p;
            j["c1"] = cmd.c1;
            break;
        }
        case arenaforge::PathCommandType::CubicTo: {
            j["c1"] = cmd.c1;
            j["c2"] = cmd.c2;
            j["p"] = cmd.p;
            break;
        }
        default:
            break;
    }
}

arenaforge::Rect adl_serializer<arenaforge::Rect>::from_json(const nlohmann::json &j) {
    auto x = as::read_value<float>(j, "x", 0.0f);
    auto y = as::read_value<float>(j, "y", 0.0f);
    auto w = as::read_value<float>(j, "w", 0.0f);
    auto h = as::read_value<float>(j, "h", 0.0f);
    return arenaforge::Rect::MakeXYWH(x, y, w, h);
}

void adl_serializer<arenaforge::Rect>::to_json(nlohmann::json &j, const arenaforge::Rect &rect) {
    j["x"] = rect.x();
    j["y"] = rect.y();
    j["w"] = rect.width();
    j["h"] = rect.height();
}

arenaforge::Point adl_serializer<arenaforge::Point>::from_json(const nlohmann::json &j) {
    auto x = as::read_value<float>(j, "x", 0.0f);
    auto y = as::read_value<float>(j, "y", 0.0f);
    return arenaforge::Point::Make(x, y);
}

void adl_serializer<arenaforge::Point>::to_json(nlohmann::json &j, const arenaforge::Point &point) {
    j["x"] = point.x;
    j["y"] = point.y;
}

};  // namespace nlohmann
