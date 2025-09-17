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
//  Log.h
//  ArenaForge
//
//  Created by king on 2025/9/17.
//

#ifndef Log_h_ArenaForge
#define Log_h_ArenaForge

#include <arenaforge_core/Print.h>

#include <cstdlib>
namespace arenaforge {

#define ABORT(msg)                                                                          \
    do {                                                                                    \
        ::arenaforge::PrintError("%s:%d: fatal error: \"%s\"\n", __FILE__, __LINE__, #msg); \
        ::abort();                                                                          \
    } while (false)

#ifdef NO_LOG

#define LOGI(...)
#define LOGE(...)
#define ASSERT(assertion)

#else

#define LOGI(...) ::arenaforge::PrintLog(__VA_ARGS__)
#define LOGE(...) ::arenaforge::PrintError(__VA_ARGS__)
#define ASSERT(assertion)  \
    if (!(assertion)) {    \
        ABORT(#assertion); \
    }

#endif

#if DEBUG

#define DEBUG_ASSERT(assertion) ASSERT(assertion)

#else

#define DEBUG_ASSERT(assertion)

#endif
};  // namespace arenaforge

#endif /* Log_h_ArenaForge */
