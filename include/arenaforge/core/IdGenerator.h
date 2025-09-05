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
//  IdGenerator.h
//  arenaforge_core
//
//  Created by king on 2025/9/5.
//

#ifndef IdGenerator_h_ArenaForge
#define IdGenerator_h_ArenaForge

#include <cstdint>

namespace arenaforge {
class IdGenerator {
  public:
    explicit IdGenerator(uint32_t start = 1)
        : _nextId(start) {}

    uint32_t generate() {
        return _nextId++;
    }

    void reset(uint32_t start = 1) {
        _nextId = start;
    }

    uint32_t peek() const {
        return _nextId;
    }

  private:
    uint32_t _nextId = 1;
};
};  // namespace arenaforge

#endif /* IdGenerator_h_ArenaForge */
