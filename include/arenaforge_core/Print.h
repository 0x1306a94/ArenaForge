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
//  Print.h
//  arenaforge_core
//
//  Created by king on 2025/9/17.
//

#ifndef Print_h_ArenaForge
#define Print_h_ArenaForge

namespace arenaforge {
/**
 * Writes an output message pointed by format to the log facility of native platform.
 */
void PrintLog(const char format[], ...);

/**
 * Writes an error message pointed by format to the log facility of native platform.
 */
void PrintError(const char format[], ...);
};  // namespace arenaforge

#endif /* Print_h_ArenaForge */
