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
//  Editor.h
//  arenaforge_editor
//
//  Created by KK on 2025/9/6.
//

#ifndef Editor_h_ArenaForge
#define Editor_h_ArenaForge

#include <arenaforge/editor/core/Platform.h>
#include <arenaforge/editor/core/defines.h>

#include <memory>

namespace arenaforge::editor {
class ARENA_FORGE_EXPORT_API Editor : public std::enable_shared_from_this<Editor> {
  public:
    static std::shared_ptr<Editor> Make();
    ~Editor();

  protected:
    Editor();

  private:
};
};  // namespace arenaforge::editor

#endif /* Editor_h_ArenaForge */
