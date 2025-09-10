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
//  AFLayer.mm
//  arenaforge
//
//  Created by KK on 2025/9/7.
//

#import <arenaforge_editor/core/AFLayer.h>

#import <arenaforge_core/layers/BaseLayer.h>
#import <arenaforge_core/uuid/UUID.h>

#include <tgfx/layers/SolidColor.h>

#import "AFLayer+Private.h"

@interface AFLayer ()
@property (nonatomic, strong) NSMutableArray<AFLayer *> *internalLayers;
@end

@implementation AFLayer {
    std::shared_ptr<arenaforge::BaseLayer> _layer;
}

#if DEBUG
- (void)dealloc {
    NSLog(@"[%@ dealloc]", NSStringFromClass(self.class));
}
#endif

- (instancetype)initWithName:(NSString *)name {
    if (self == [super init]) {
        auto uuid = arenaforge::UUID::Instance();
        auto layerId = uuid();

        _layer = arenaforge::BaseLayer::Make(layerId, arenaforge::ShapeType::Rectangle);
        _layer->setName((name == nil ? "" : std::string(name.UTF8String)));
        _internalLayers = [NSMutableArray<AFLayer *> array];
    }
    return self;
}

- (instancetype)initWithCppObject:(std::shared_ptr<arenaforge::BaseLayer>)cppObject {
    if (self == [super init]) {
        _layer = std::move(cppObject);
        _internalLayers = [NSMutableArray<AFLayer *> array];
        for (auto &cppChild : _layer->children()) {
            AFLayer *child = [[AFLayer alloc] initWithCppObject:std::static_pointer_cast<arenaforge::BaseLayer>(cppChild)];
            [_internalLayers addObject:child];
        }
    }
    return self;
}

- (void)addChild:(AFLayer *)child {
    if (child == nil) {
        return;
    }

    auto cppObject = [child cppObject];
    if (cppObject->parent() == _layer.get()) {
        return;
    }

    _layer->addChild(cppObject);
    [self.internalLayers addObject:child];
}

#pragma mark - setter getter

- (std::shared_ptr<arenaforge::BaseLayer>)cppObject {
    return _layer;
}

- (NSString *)venueId {
    auto layerId = _layer->layerId();
    return [NSString stringWithUTF8String:layerId.c_str()];
}

- (NSString *)name {
    auto name = _layer->name();
    return [NSString stringWithUTF8String:name.c_str()];
}

- (NSArray<AFLayer *> *)children {
    return [self.internalLayers copy];
}

- (BOOL)hasChildren {
    return self.internalLayers.count > 0;
}

- (NSInteger)childrenCount {
    return (NSInteger)self.internalLayers.count;
}

- (void)setTransient:(BOOL)transient {
    _layer->setTransient(transient);
}

- (BOOL)transient {
    return _layer->transient();
}

- (void)setFrame:(NSRect)frame {
    auto cppRect = tgfx::Rect::MakeXYWH(
        static_cast<float>(frame.origin.x),
        static_cast<float>(frame.origin.y),
        static_cast<float>(frame.size.width),
        static_cast<float>(frame.size.height));
    _layer->setFrame(cppRect);
}

- (NSRect)frame {
    auto cppRect = _layer->frame();
    return NSRectFromCGRect(CGRectMake(cppRect.x(), cppRect.y(), cppRect.width(), cppRect.height()));
}

- (void)setFillColor:(NSColor *)fillColor {
    if (fillColor == nil || fillColor == NSColor.clearColor) {
        _layer->setFillStyle(nullptr);
        return;
    }

    CGFloat red, green, blue, alpha;
    [fillColor getRed:&red green:&green blue:&blue alpha:&alpha];
    auto cppColor = tgfx::Color{
        static_cast<float>(red),
        static_cast<float>(green),
        static_cast<float>(blue),
        static_cast<float>(alpha)};

    _layer->setFillStyle(tgfx::SolidColor::Make(cppColor));
}

- (NSColor *)fillColor {
    auto fillStyles = _layer->fillStyles();
    if (fillStyles.empty()) {
        return nil;
    }
    auto fill = std::static_pointer_cast<tgfx::SolidColor>(fillStyles.front());
    auto cppColor = fill->color();
    if (cppColor == tgfx::Color::Transparent()) {
        return nil;
    }
    return [NSColor colorWithRed:cppColor.red green:cppColor.green blue:cppColor.blue alpha:cppColor.alpha];
}
@end
