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
//  ProjectShapeCounter.h
//  arenaforge_core
//
//  Created by KK on 2025/9/18.
//

#ifndef ProjectShapeCounter_h_ArenaForge
#define ProjectShapeCounter_h_ArenaForge

#include <cstdint>

namespace arenaforge {
struct ProjectShapeCounter {

    uint32_t nextRectangle() {
        return _rectangle++;
    }

    uint32_t nextGroup() {
        return _group++;
    }

    uint32_t nextLine() {
        return _line++;
    }

    uint32_t nextEllipse() {
        return _ellipse++;
    }

  private:
    uint32_t _rectangle{0};
    uint32_t _group{0};
    uint32_t _line{0};
    uint32_t _ellipse{0};
};
};  // namespace arenaforge

#endif /* ProjectShapeCounter_h_ArenaForge */
