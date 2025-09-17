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
//  AFProject+Private.h
//  arenaforge_editor
//
//  Created by KK on 2025/9/7.
//

#import <arenaforge_editor/core/AFProject.h>

#import <arenaforge_core/Project.h>

NS_ASSUME_NONNULL_BEGIN

@class AFLayerMap;
@interface AFProject ()
@property (nonatomic, strong, readonly) AFLayerMap *layerMap;
@property (nonatomic, strong, nullable) void (^venueChangeHandler)(AFProject *project);
- (std::shared_ptr<arenaforge::Project>)cppObject;
@end

NS_ASSUME_NONNULL_END
