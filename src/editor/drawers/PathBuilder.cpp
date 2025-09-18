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
//  PathBuilder.cpp
//  arenaforge_editor
//
//  Created by king on 2025/9/18.
//

#include "PathBuilder.h"

namespace arenaforge {
tgfx::Path PathBuilder::BuildPath(const std::vector<PathCommand> &commands, const Size &size) {
    tgfx::Path path;
    for (const auto &command : commands) {
        switch (command.type) {
            case PathCommandType::MoveTo: {
                auto p = command.p * size;
                path.moveTo(tgfx::Point{p.x, p.y});
                break;
            }
            case PathCommandType::LineTo: {
                auto p = command.p * size;
                path.lineTo(tgfx::Point{p.x, p.y});
                break;
            }
            case PathCommandType::QuadTo: {
                auto c = command.c1 * size;
                auto p = command.p * size;
                path.quadTo(tgfx::Point{c.x, c.y}, tgfx::Point{p.x, p.y});
                break;
            }
            case PathCommandType::CubicTo: {
                auto c1 = command.c1 * size;
                auto c2 = command.c2 * size;
                auto p = command.p * size;
                path.cubicTo(tgfx::Point{c1.x, c1.y}, tgfx::Point{c2.x, c2.y}, tgfx::Point{p.x, p.y});
                break;
            }
            case PathCommandType::ClosePath: {
                path.close();
                break;
            }
            default:
                break;
        }
    }
    return path;
}
};  // namespace arenaforge
