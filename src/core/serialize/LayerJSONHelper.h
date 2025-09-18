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
//  LayerJSONHelper.h
//  arenaforge_core
//
//  Created by king on 2025/9/17.
//

#ifndef LayerJSONHelper_h_ArenaForge
#define LayerJSONHelper_h_ArenaForge

#include <nlohmann/json.hpp>

namespace arenaforge {
class Layer;
class ShapeLayer;
class BooleanLayer;
struct Color;
struct Point;
struct ISize;
struct Size;
struct Rect;
struct PathCommand;

};  // namespace arenaforge

namespace nlohmann {
template <>
struct adl_serializer<std::shared_ptr<arenaforge::Layer>> {
    static std::shared_ptr<arenaforge::Layer> from_json(const nlohmann::json &j);
    static void to_json(nlohmann::json &j, std::shared_ptr<arenaforge::Layer> layer);
};

template <>
struct adl_serializer<arenaforge::PathCommand> {
    static arenaforge::PathCommand from_json(const nlohmann::json &j);
    static void to_json(nlohmann::json &j, const arenaforge::PathCommand &cmd);
};

template <>
struct adl_serializer<arenaforge::Rect> {
    static arenaforge::Rect from_json(const nlohmann::json &j);
    static void to_json(nlohmann::json &j, const arenaforge::Rect &rect);
};

template <>
struct adl_serializer<arenaforge::Point> {
    static arenaforge::Point from_json(const nlohmann::json &j);
    static void to_json(nlohmann::json &j, const arenaforge::Point &color);
};
};  // namespace nlohmann

#endif /* LayerJSONHelper_h_ArenaForge */
