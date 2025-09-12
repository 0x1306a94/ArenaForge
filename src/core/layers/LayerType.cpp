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
//  LayerType.cpp
//  arenaforge_core
//
//  Created by KK on 2025/9/12.
//

#include <arenaforge_core/layers/LayerType.h>

namespace arenaforge {
std::string LayerTypeToString(LayerType type) {
    switch (type) {
        case LayerType::Rectangle:
            return "rectangle";
        case LayerType::Ellipse:
            return "ellipse";
        case LayerType::Line:
            return "line";
        case LayerType::Triangle:
            return "triangle";
        case LayerType::Vector:
            return "vector";
        default:
            break;
    }
    return "group";
}

LayerType LayerTypeFromString(const std::string &type) {
    if (type == "rectangle") {
        return LayerType::Rectangle;
    } else if (type == "ellipse") {
        return LayerType::Ellipse;
    } else if (type == "line") {
        return LayerType::Line;
    } else if (type == "triangle") {
        return LayerType::Triangle;
    } else if (type == "vector") {
        return LayerType::Vector;
    }
    return LayerType::Group;
}
};  // namespace arenaforge
