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
//  BaseLayerJSONHelper.h
//  arenaforge_core
//
//  Created by king on 2025/9/10.
//

#ifndef BaseLayerJSONHelper_h_ArenaForge
#define BaseLayerJSONHelper_h_ArenaForge

#include <arenaforge_core/layers/BaseLayer.h>

#include <nlohmann/json.hpp>

namespace tgfx {
struct Color;
struct Rect;
struct Point;
class SolidColor;
}  // namespace tgfx

namespace nlohmann {
template <>
struct adl_serializer<std::shared_ptr<arenaforge::BaseLayer>> {
    static std::shared_ptr<arenaforge::BaseLayer> from_json(const nlohmann::json &j);
    static void to_json(nlohmann::json &j, std::shared_ptr<arenaforge::BaseLayer> layer);
};

template <>
struct adl_serializer<arenaforge::PathCommand> {
    static arenaforge::PathCommand from_json(const nlohmann::json &j);
    static void to_json(nlohmann::json &j, const arenaforge::PathCommand &cmd);
};

template <>
struct adl_serializer<std::shared_ptr<tgfx::SolidColor>> {
    static std::shared_ptr<tgfx::SolidColor> from_json(const nlohmann::json &j);
    static void to_json(nlohmann::json &j, std::shared_ptr<tgfx::SolidColor> style);
};

template <>
struct adl_serializer<tgfx::Color> {
    static tgfx::Color from_json(const nlohmann::json &j);
    static void to_json(nlohmann::json &j, const tgfx::Color &color);
};

template <>
struct adl_serializer<tgfx::Rect> {
    static tgfx::Rect from_json(const nlohmann::json &j);
    static void to_json(nlohmann::json &j, const tgfx::Rect &rect);
};

template <>
struct adl_serializer<tgfx::Point> {
    static tgfx::Point from_json(const nlohmann::json &j);
    static void to_json(nlohmann::json &j, const tgfx::Point &color);
};
};  // namespace nlohmann

#endif /* BaseLayerJSONHelper_h_ArenaForge */
