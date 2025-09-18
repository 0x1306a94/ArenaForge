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
//  AFProject.h
//  arenaforge
//
//  Created by KK on 2025/9/7.
//

#ifndef AFProject_h_ArenaForge
#define AFProject_h_ArenaForge

#import <CoreFoundation/CFCGTypes.h>
#import <Foundation/Foundation.h>

#import <arenaforge_editor/bridge/apple/AFBuiltinShapeType.h>
#import <arenaforge_editor/core/defines.h>

NS_ASSUME_NONNULL_BEGIN

@class AFLayer;
@class AFShapeLayer;
ARENA_FORGE_EXPORT_API @interface AFProject : NSObject
@property (nonatomic, copy, readonly) NSString *name;
@property (nonatomic, strong, readonly) AFLayer *root;
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithName:(NSString *)name description:(NSString *)description width:(CGFloat)width height:(CGFloat)height NS_DESIGNATED_INITIALIZER;
- (instancetype __nullable)initWithFileURL:(NSURL *)fileURL error:(NSError **)error NS_DESIGNATED_INITIALIZER;

- (NSString *)toJSONString;

- (AFLayer *_Nullable)createLayer;

- (AFShapeLayer *_Nullable)createShapeLayer:(AFBuiltinShapeType)type;

- (AFLayer *_Nullable)findLayerById:(NSString *)layerId;

- (AFLayer *_Nullable)upgradeGroup:(NSArray<AFLayer *> *)layers;

- (BOOL)undoGroup:(AFLayer *)group;
@end

NS_ASSUME_NONNULL_END

#endif /* AFProject_h_ArenaForge */
