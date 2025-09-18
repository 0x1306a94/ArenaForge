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
//  AFLayer.h
//  arenaforge
//
//  Created by KK on 2025/9/7.
//

#ifndef AFLayer_h_ArenaForge
#define AFLayer_h_ArenaForge

#import <Foundation/Foundation.h>

#import <arenaforge_editor/bridge/apple/AFLayerType.h>

#import <arenaforge_editor/core/defines.h>

NS_ASSUME_NONNULL_BEGIN

@class AFLayerMap;
ARENA_FORGE_EXPORT_API @interface AFLayer : NSObject
@property (nonatomic, copy, readonly) NSString *layerId;
@property (nonatomic, assign, readonly) AFLayerType type;
@property (nonatomic, copy) NSString *name;
@property (nonatomic, copy, readonly) NSArray<AFLayer *> *children;
@property (nonatomic, assign, readonly) NSInteger childrenCount;
@property (nonatomic, assign, readonly) BOOL hasChildren;
@property (nonatomic, assign) BOOL transient;
@property (nonatomic, assign) NSRect frame;
@property (nonatomic, weak, readonly) AFLayer *parent;
@property (nonatomic, assign, readonly) BOOL isRoot;
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithName:(NSString *)name layerMap:(AFLayerMap *)layerMap NS_DESIGNATED_INITIALIZER;

- (BOOL)addChild:(AFLayer *)child;
- (BOOL)addChild:(AFLayer *)child atIndex:(int)index;
- (int)getChildIndex:(AFLayer *)child;
- (void)removeChild:(AFLayer *)child;
- (void)removeFromParent;
@end

NS_ASSUME_NONNULL_END

#endif /* AFLayer_h_ArenaForge */
