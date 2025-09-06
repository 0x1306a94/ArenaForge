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
//  PathCommand.cpp
//  arenaforge_core
//
//  Created by KK on 2025/9/6.
//

#include <arenaforge/core/PathCommand.h>

namespace arenaforge {

PathCommand PathCommand::MakeMoveTo(const tgfx::Point &point) {
    PathCommand cmd{};
    cmd.type = PathCommandType::MoveTo;
    cmd.cmd.moveTo.p = point;
    return cmd;
}

PathCommand PathCommand::MakeLineTo(const tgfx::Point &point) {
    PathCommand cmd{};
    cmd.type = PathCommandType::LineTo;
    cmd.cmd.lineTo.p = point;
    return cmd;
}

PathCommand PathCommand::MakeQuadTo(const tgfx::Point &control, const tgfx::Point &point) {
    PathCommand cmd{};
    cmd.type = PathCommandType::QuadTo;
    cmd.cmd.quadTo.c = control;
    cmd.cmd.quadTo.p = point;
    return cmd;
}

PathCommand PathCommand::MakeCubicTo(const tgfx::Point &controlStart, const tgfx::Point &controlEnd, const tgfx::Point &point) {
    PathCommand cmd{};
    cmd.type = PathCommandType::CubicTo;
    cmd.cmd.cubicTo.c1 = controlStart;
    cmd.cmd.cubicTo.c2 = controlEnd;
    cmd.cmd.cubicTo.p = point;
    return cmd;
}

PathCommand PathCommand::MakeClose() {
    PathCommand cmd{};
    cmd.type = PathCommandType::ClosePath;
    return cmd;
}

std::string PathCommandTypeToString(PathCommandType type) {
    switch (type) {
        case PathCommandType::MoveTo:
            return "moveTo";
        case PathCommandType::LineTo:
            return "lineTo";
        case PathCommandType::QuadTo:
            return "quadTo";
        case PathCommandType::CubicTo:
            return "cubicTo";
        case PathCommandType::ClosePath:
            return "close";
        default:
            return "unknown";
            break;
    }
}

PathCommandType PathCommandTypeFromString(const std::string &type) {
    if (type == "moveTo") {
        return PathCommandType::MoveTo;
    } else if (type == "lineTo") {
        return PathCommandType::LineTo;
    } else if (type == "quadTo") {
        return PathCommandType::QuadTo;
    } else if (type == "cubicTo") {
        return PathCommandType::CubicTo;
    } else if (type == "close") {
        return PathCommandType::ClosePath;
    }
    return PathCommandType::Unknown;
}
};  // namespace arenaforge
