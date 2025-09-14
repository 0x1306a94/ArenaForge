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
#import "AFLayerMap.h"
#import "AFVenue+Private.h"

#import <AppKit/NSColorSpace.h>

@interface AFLayer ()
@property (nonatomic, weak) AFLayer *parent;
@property (nonatomic, weak) AFLayerMap *layerMap;
@property (nonatomic, strong) NSArray<AFLayer *> *cacheChildren;
@end

@implementation AFLayer {
    std::shared_ptr<arenaforge::BaseLayer> _layer;
}

//#if DEBUG
//- (void)dealloc {
//    NSLog(@"[%@ dealloc]", NSStringFromClass(self.class));
//}
//#endif

- (instancetype)initWithName:(NSString *)name type:(AFLayerType)type layerMap:(AFLayerMap *)layerMap {
    if (self == [super init]) {
        auto uuid = arenaforge::UUID::Instance();
        auto layerId = uuid();
        _layerMap = layerMap;
        _layer = arenaforge::BaseLayer::Make(layerId, static_cast<arenaforge::LayerType>(type));
        _layer->setName((name == nil ? "" : std::string(name.UTF8String)));

        _cacheChildren = @[];
    }
    return self;
}

- (instancetype)initWithCppObject:(std::shared_ptr<arenaforge::BaseLayer>)cppObject layerMap:(AFLayerMap *)layerMap {
    if (self == [super init]) {
        _layer = std::move(cppObject);
        _layerMap = layerMap;
        for (auto &cppChild : _layer->children()) {
            AFLayer *child = [[AFLayer alloc] initWithCppObject:std::static_pointer_cast<arenaforge::BaseLayer>(cppChild) layerMap:layerMap];
            child.parent = self;
            [layerMap addLayer:child];
        }

        [self rebuildCacheChildren];
    }
    return self;
}

- (BOOL)addChild:(AFLayer *)child {
    if (child == nil) {
        return NO;
    }
    auto cppObject = [child cppObject];

    auto index = _layer->children().size();
    if (cppObject->parent() == _layer.get()) {
        index--;
    }

    return [self addChild:child atIndex:static_cast<int>(index)];
}

- (BOOL)addChild:(AFLayer *)child atIndex:(int)index {
    if (child == nil) {
        return NO;
    }

    auto cppObject = [child cppObject];

    if (!_layer->addChildAt(cppObject, index)) {
        return NO;
    }

    [child.parent rebuildCacheChildren];

    child.parent = self;
    [self rebuildCacheChildren];

    return YES;
}

- (int)getChildIndex:(AFLayer *)child {
    if (child == nil) {
        return -1;
    }
    auto cppObject = [child cppObject];
    auto index = _layer->getChildIndex(cppObject);
    return index;
}

- (void)removeChild:(AFLayer *)child {
    if (!child) {
        return;
    }

    auto cppObject = [child cppObject];
    cppObject->removeFromParent();
    [child.parent rebuildCacheChildren];
    child.parent = nil;
}

- (void)removeFromParent {
    _layer->removeFromParent();
    [self.parent rebuildCacheChildren];
    self.parent = nil;
}

- (NSPoint)globalToLocal:(NSPoint)point {
    auto local = _layer->globalToLocal(tgfx::Point{static_cast<float>(point.x), static_cast<float>(point.y)});
    return NSPointFromCGPoint(CGPointMake(local.x, local.y));
}

- (NSPoint)localToGlobal:(NSPoint)point {
    auto global = _layer->localToGlobal(tgfx::Point{static_cast<float>(point.x), static_cast<float>(point.y)});
    return NSPointFromCGPoint(CGPointMake(global.x, global.y));
}

- (void)rebuildCacheChildren {
    NSMutableArray<AFLayer *> *children = [NSMutableArray<AFLayer *> array];
    for (const auto &item : _layer->children()) {
        auto baseLayer = std::static_pointer_cast<arenaforge::BaseLayer>(item);
        NSString *layerId = [NSString stringWithUTF8String:baseLayer->layerId().c_str()];
        AFLayer *layer = [self.layerMap getLayerById:layerId];
        if (layer) {
            [children addObject:layer];
        }
    }
    self.cacheChildren = [children copy];
}
#pragma mark - setter getter

- (std::shared_ptr<arenaforge::BaseLayer>)cppObject {
    return _layer;
}

- (NSString *)layerId {
    auto layerId = _layer->layerId();
    return [NSString stringWithUTF8String:layerId.c_str()];
}

- (AFLayerType)type {
    auto type = static_cast<AFLayerType>(_layer->userType());
    return type;
}

- (NSString *)name {
    auto name = _layer->name();
    return [NSString stringWithUTF8String:name.c_str()];
}

- (void)setName:(NSString *)name {
    _layer->setName((name == nil ? "" : name.UTF8String));
}

- (NSArray<AFLayer *> *)children {
    return self.cacheChildren;
}

- (BOOL)hasChildren {
    auto count = self.cacheChildren.count;
    return count > 0;
}

- (NSInteger)childrenCount {
    NSInteger count = (NSInteger)self.cacheChildren.count;
    return count;
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

- (void)setPositionRelative:(BOOL)positionRelative {
    _layer->setPositionRelative(positionRelative);
}

- (BOOL)positionRelative {
    return _layer->positionRelative();
}

- (void)setFillColor:(NSColor *)fillColor {
    if (fillColor == nil) {
        _layer->setFillStyle(nullptr);
        return;
    }

    NSColor *rgbColor = [fillColor colorUsingColorSpace:[NSColorSpace sRGBColorSpace]];
    CGFloat red, green, blue, alpha;
    [rgbColor getRed:&red green:&green blue:&blue alpha:&alpha];
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
        return NSColor.clearColor;
    }
    return [NSColor colorWithRed:cppColor.red green:cppColor.green blue:cppColor.blue alpha:cppColor.alpha];
}

- (void)setStrokeColor:(NSColor *)strokeColor {
    if (strokeColor == nil) {
        _layer->setStrokeStyle(nullptr);
        return;
    }

    NSColor *rgbColor = [strokeColor colorUsingColorSpace:[NSColorSpace sRGBColorSpace]];
    CGFloat red, green, blue, alpha;
    [rgbColor getRed:&red green:&green blue:&blue alpha:&alpha];
    auto cppColor = tgfx::Color{
        static_cast<float>(red),
        static_cast<float>(green),
        static_cast<float>(blue),
        static_cast<float>(alpha)};

    _layer->setStrokeStyle(tgfx::SolidColor::Make(cppColor));
}

- (NSColor *)strokeColor {
    auto strokeStyles = _layer->strokeStyles();
    if (strokeStyles.empty()) {
        return nil;
    }
    auto stroke = std::static_pointer_cast<tgfx::SolidColor>(strokeStyles.front());
    auto cppColor = stroke->color();
    if (cppColor == tgfx::Color::Transparent()) {
        return NSColor.clearColor;
    }
    return [NSColor colorWithRed:cppColor.red green:cppColor.green blue:cppColor.blue alpha:cppColor.alpha];
}

- (void)setLineWidth:(CGFloat)lineWidth {
    _layer->setLineWidth(static_cast<float>(lineWidth));
}

- (CGFloat)lineWidth {
    return static_cast<CGFloat>(_layer->lineWidth());
}

- (AFVenue *)attachVenue {
    AFLayer *current = self;
    while (current) {
        AFVenue *venue = current.venue;
        if (venue) {
            return venue;
        }
        current = current.parent;
    }
    return nil;
}
@end
