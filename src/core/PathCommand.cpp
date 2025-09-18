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

#include <arenaforge_core/PathCommand.h>

namespace arenaforge {

PathCommand Make(PathCommandType type) {
    PathCommand cmd{};
    cmd.type = type;
    return cmd;
}

PathCommand PathCommand::MakeMoveTo(const Point &point) {
    PathCommand cmd{};
    cmd.type = PathCommandType::MoveTo;
    cmd.c1 = point;
    cmd.c2 = point;
    cmd.p = point;
    return cmd;
}

PathCommand PathCommand::MakeLineTo(const Point &point) {
    PathCommand cmd{};
    cmd.type = PathCommandType::LineTo;
    cmd.c1 = point;
    cmd.c2 = point;
    cmd.p = point;
    return cmd;
}

PathCommand PathCommand::MakeQuadTo(const Point &control, const Point &point) {
    PathCommand cmd{};
    cmd.type = PathCommandType::QuadTo;
    cmd.c1 = control;
    cmd.c2 = control;
    cmd.p = point;
    return cmd;
}

PathCommand PathCommand::MakeCubicTo(const Point &controlStart, const Point &controlEnd, const Point &point) {
    PathCommand cmd{};
    cmd.type = PathCommandType::CubicTo;
    cmd.c1 = controlStart;
    cmd.c2 = controlEnd;
    cmd.p = point;
    return cmd;
}

PathCommand PathCommand::MakeClose() {
    PathCommand cmd{};
    cmd.type = PathCommandType::ClosePath;
    return cmd;
}

std::vector<PathCommand> PathCommand::MakeRectangle() {
    std::vector<PathCommand> commands;
    commands.push_back(PathCommand::MakeMoveTo({0.0, 0.0}));
    commands.push_back(PathCommand::MakeLineTo({1.0, 0.0}));
    commands.push_back(PathCommand::MakeLineTo({1.0, 1.0}));
    commands.push_back(PathCommand::MakeLineTo({0.0, 1.0}));
    commands.push_back(PathCommand::MakeLineTo({0.0, 0.0}));
    commands.push_back(PathCommand::MakeClose());
    return commands;
}

std::vector<PathCommand> PathCommand::MakeTriangle() {
    std::vector<PathCommand> commands;
    commands.push_back(PathCommand::MakeMoveTo({0.5, 0.0}));
    commands.push_back(PathCommand::MakeLineTo({1.0, 1.0}));
    commands.push_back(PathCommand::MakeLineTo({0.0, 1.0}));
    commands.push_back(PathCommand::MakeLineTo({0.5, 0.0}));
    commands.push_back(PathCommand::MakeClose());
    return commands;
}

std::vector<PathCommand> PathCommand::MakeEllipse() {
    std::vector<PathCommand> commands;

    // 椭圆的中心和半径（这里默认椭圆位于 (0,0)-(1,1) 的矩形内）
    float cx = 0.5f;
    float cy = 0.5f;
    float rx = 0.5f;  // 半径 x
    float ry = 0.5f;  // 半径 y

    // 贝塞尔逼近圆弧的常数 (float)
    const float kappa = 0.55228475f;

    // 从右边点开始
    commands.push_back(PathCommand::MakeMoveTo({cx + rx, cy}));

    // 右 -> 下
    commands.push_back(PathCommand::MakeCubicTo(
        {cx + rx, cy + ry * kappa},
        {cx + rx * kappa, cy + ry},
        {cx, cy + ry}));

    // 下 -> 左
    commands.push_back(PathCommand::MakeCubicTo(
        {cx - rx * kappa, cy + ry},
        {cx - rx, cy + ry * kappa},
        {cx - rx, cy}));

    // 左 -> 上
    commands.push_back(PathCommand::MakeCubicTo(
        {cx - rx, cy - ry * kappa},
        {cx - rx * kappa, cy - ry},
        {cx, cy - ry}));

    // 上 -> 右
    commands.push_back(PathCommand::MakeCubicTo(
        {cx + rx * kappa, cy - ry},
        {cx + rx, cy - ry * kappa},
        {cx + rx, cy}));

    commands.push_back(PathCommand::MakeClose());

    return commands;
}

std::vector<PathCommand> PathCommand::MakeLine() {
    std::vector<PathCommand> commands;
    commands.push_back(PathCommand::MakeMoveTo({0.5, 0.5}));
    commands.push_back(PathCommand::MakeLineTo({1.0, 0.5}));
    return commands;
}

std::vector<PathCommand> PathCommand::MakeFrom(BuiltinShapeType type) {
    switch (type) {
        case BuiltinShapeType::Rectangle:
            return MakeRectangle();
        case BuiltinShapeType::Triangle:
            return MakeTriangle();
        case BuiltinShapeType::Ellipse:
            return MakeEllipse();
        case BuiltinShapeType::Line:
            return MakeLine();
    }
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
