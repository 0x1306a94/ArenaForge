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
//  arenaforge_editor.h
//  ArenaForge
//
//  Created by KK on 2025/9/6.
//

#ifndef arenaforge_editor_h_ArenaForge
#define arenaforge_editor_h_ArenaForge

#include <arenaforge_editor/core/Platform.h>

#if defined(__APPLE__)

#import <Foundation/Foundation.h>

#import <arenaforge_editor/bridge/apple/AFEditor.h>
#import <arenaforge_editor/bridge/apple/AFLayer.h>
#import <arenaforge_editor/bridge/apple/AFShapeLayer.h>
#import <arenaforge_editor/bridge/apple/AFBooleanLayer.h>
#import <arenaforge_editor/bridge/apple/AFProject.h>

#if defined(TARGET_OS_OSX)
#import <arenaforge_editor/bridge/mac/AFMacCanvasView.h>
#endif

#else

#include <arenaforge_editor/core/Editor.h>

#endif

#endif /* arenaforge_editor_h_ArenaForge */
