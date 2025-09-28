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
//  PathCommand.h
//  arenaforge_core
//
//  Created by KK on 2025/9/6.
//

#ifndef PathCommand_h_ArenaForge
#define PathCommand_h_ArenaForge

#include <string>
#include <vector>

#include <arenaforge_core/BuiltinShapeType.h>
#include <arenaforge_core/Point.h>

namespace arenaforge {
enum class PathCommandType {
    Unknown,
    MoveTo,
    LineTo,
    QuadTo,
    CubicTo,
    ClosePath
};

struct PathCommand {
    PathCommandType type;
    Point c1, c2, p;

    static PathCommand Make(PathCommandType type);
    static PathCommand MakeMoveTo(const Point &point);
    static PathCommand MakeLineTo(const Point &point);
    static PathCommand MakeQuadTo(const Point &control, const Point &point);
    static PathCommand MakeCubicTo(const Point &controlStart, const Point &controlEnd, const Point &point);
    static PathCommand MakeClose();

    static std::vector<PathCommand> MakeRectangle();
    static std::vector<PathCommand> MakeTriangle();
    static std::vector<PathCommand> MakeEllipse();
    static std::vector<PathCommand> MakeLine();
    static std::vector<PathCommand> MakeFrom(BuiltinShapeType type);

    static bool isLine(const std::vector<PathCommand> &commands);
};

std::string PathCommandTypeToString(PathCommandType type);
PathCommandType PathCommandTypeFromString(const std::string &type);

};  // namespace arenaforge

#endif /* PathCommand_h_ArenaForge */
