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
//  AFEditor.h
//  arenaforge
//
//  Created by KK on 2025/9/6.
//

#ifndef AFEditor_h_ArenaForge
#define AFEditor_h_ArenaForge

#import <Foundation/Foundation.h>

#import <arenaforge_editor/core/defines.h>

NS_ASSUME_NONNULL_BEGIN

@class AFMacCanvasView;
@class AFLayer;
@class AFProject;
@class AFVenue;
ARENA_FORGE_EXPORT_API @interface AFEditor : NSObject
@property (nonatomic, strong, readonly) AFProject *project;
+ (instancetype)new NS_UNAVAILABLE;
- (instancetype)init NS_UNAVAILABLE;
- (instancetype)initWithProject:(AFProject *)project NS_DESIGNATED_INITIALIZER;
- (void)setupCanvasView:(AFMacCanvasView *)canvasView;

- (CGFloat)zoomScale;

- (NSPoint)contentOffset;

- (CGFloat)density;

- (void)autoAdjustCanvasScaleForContent;
- (void)updateZoomScale:(CGFloat)zoomScale;
- (void)updateOffset:(NSPoint)offset;
- (void)updateZoomScale:(CGFloat)zoomScale offset:(NSPoint)offset;

@end

NS_ASSUME_NONNULL_END

#endif /* AFEditor_h_ArenaForge */
