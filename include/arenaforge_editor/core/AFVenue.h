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
//  AFVenue.h
//  arenaforge
//
//  Created by KK on 2025/9/7.
//

#ifndef AFVenue_h_ArenaForge
#define AFVenue_h_ArenaForge

#import <AppKit/NSColor.h>
#import <Foundation/Foundation.h>

#import <arenaforge_editor/core/defines.h>

NS_ASSUME_NONNULL_BEGIN

@class AFLayer;

ARENA_FORGE_EXPORT_API @interface AFVenue : NSObject
@property (nonatomic, copy, readonly) NSString *venueId;
@property (nonatomic, copy) NSString *name;
@property (nonatomic, strong, readonly) AFLayer *root;
@property (nonatomic, assign) NSRect frame;
@property (nonatomic, strong) NSColor *backgroundColor;

+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithName:(NSString *)name NS_DESIGNATED_INITIALIZER;

- (NSString *)toJSONString;

- (AFLayer *_Nullable)findLayerById:(NSString *)layerId;
- (AFLayer *_Nullable)upgradeGroup:(NSArray<AFLayer *> *)layers;
- (BOOL)undoGroup:(AFLayer *)group;
- (AFLayer *_Nullable)pickVenueAtUnderPoint:(NSPoint)point;
- (void)resetHoverWireframe;
- (BOOL)hitTestPoint:(NSPoint)point;
- (NSPoint)globalToLocal:(NSPoint)point;
- (NSPoint)localToGlobal:(NSPoint)point;

@end

NS_ASSUME_NONNULL_END

#endif /* AFVenue_h_ArenaForge */
