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
//  AFVenue.mm
//  arenaforge
//
//  Created by KK on 2025/9/7.
//

#import <arenaforge_editor/core/AFVenue.h>

#import <arenaforge_editor/core/AFLayer.h>

#import <arenaforge_core/Project.h>
#import <arenaforge_core/Venue.h>
#import <arenaforge_core/uuid/UUID.h>

#import "AFLayer+Private.h"
#import "AFProject+Private.h"
#import "AFVenue+Private.h"

#import "AFLayerMap.h"

#import <AppKit/NSColorSpace.h>

#import <tgfx/layers/SolidColor.h>

@interface AFVenue ()
@property (nonatomic, strong) AFLayer *root;
@end

@implementation AFVenue {
    std::shared_ptr<arenaforge::Venue> _venue;
}
#if DEBUG
- (void)dealloc {
    NSLog(@"[%@ dealloc]", NSStringFromClass(self.class));
}
#endif

- (instancetype)initWithName:(NSString *)name {
    if (self == [super init]) {

        _layerMap = [AFLayerMap new];

        auto uuid = arenaforge::UUID::Instance();
        auto venueId = uuid();
        _venue = arenaforge::Venue::Make(venueId, (name == nil ? "" : std::string(name.UTF8String)), "");
        _venue->setBackgroundColor(tgfx::Color::FromRGBA(0xcc, 0xcc, 0xcc));

        _root = [[AFLayer alloc] initWithCppObject:_venue->containerLayerPtr() layerMap:_layerMap];
        _root.venue = self;
    }
    return self;
}

- (instancetype)initWithCppObject:(std::shared_ptr<arenaforge::Venue>)cppObject {
    if (self == [super init]) {
        _venue = std::move(cppObject);

        _layerMap = [AFLayerMap new];

        _root = [[AFLayer alloc] initWithCppObject:_venue->containerLayerPtr() layerMap:_layerMap];
        _root.venue = self;
    }
    return self;
}

- (std::shared_ptr<arenaforge::Venue>)cppObject {
    return _venue;
}

- (AFLayer *)addHoverWireframeLayer:(AFLayer *)target {
    if (target == nil) {
        return nil;
    }
    auto targetLayer = [target cppObject];
    _venue->addHoverWireframe({targetLayer});
    //    auto rootLayer = _venue->rootLayerPtr();
    //    auto containerLayer = _venue->containerLayer();
    //    auto targetLayer = [target cppObject];
    //    auto parent = targetLayer->parent();
    //    if (parent == nullptr) {
    //        return nil;
    //    }
    //
    //    NSRect globalRect;
    //    if (targetLayer.get() == containerLayer) {
    //        auto venueFrame = _venue->frame();
    //        globalRect = NSRectFromCGRect(CGRectMake(venueFrame.x(), venueFrame.y(), venueFrame.width(), venueFrame.height()));
    //    } else {
    //        auto targetFrame = targetLayer->frame();
    //        auto global = targetLayer->localToGlobal(tgfx::Point{0, 0});
    //        auto local = rootLayer->globalToLocal(global);
    //        globalRect = NSRectFromCGRect(CGRectMake(local.x, local.y, targetFrame.width(), targetFrame.height()));
    //    }
    //
    //    auto hoverLayer = targetLayer->clone(false);
    //    if (hoverLayer == nullptr) {
    //        return nil;
    //    }
    //
    //    hoverLayer->setPosition(tgfx::Point{static_cast<float>(globalRect.origin.x), static_cast<float>(globalRect.origin.y)});
    //    hoverLayer->setFillStyle(nullptr);
    //    hoverLayer->setLineWidth(4);
    //    hoverLayer->setStrokeStyle(tgfx::SolidColor::Make(tgfx::Color::FromRGBA(0x0c, 0x8c, 0xe9)));
    //
    //    AFLayer *layer = [[AFLayer alloc] initWithCppObject:hoverLayer layerMap:self.layerMap];
    //    AFLayer *layer = [[AFLayer alloc] initWithName:@"HoverWireframe" type:AFLayerTypeRectangle layerMap:self.layerMap];
    //    layer.positionRelative = NO;
    //    layer.frame = globalRect;
    // tgfx::Color::FromRGBA(0x0c, 0x8c, 0xe9)
    //    layer.strokeColor = [NSColor colorWithRed:(0x0c / 255.0) green:(0x8c / 255.0) blue:(0xe9 / 255.0) alpha:1.0];
    //    layer.lineWidth = 4;

    //    auto cppLayer = [layer cppObject];
    //    if (targetLayer.get() == containerLayer) {
    //        rootLayer->parent()->addChild(hoverLayer);
    //    } else {
    //        rootLayer->addChild(hoverLayer);
    //    }

    return nil;
}

- (NSString *)toJSONString {
    auto json = _venue->toJSON();
    return [NSString stringWithUTF8String:json.c_str()];
}

- (AFLayer *_Nullable)findLayerById:(NSString *)layerId {
    AFLayer *cache = [self.layerMap getLayerById:layerId];
    if (cache) {
        return cache;
    }

    //    if (!self.root) {
    //        return nil;
    //    }
    //
    //    NSMutableArray<AFLayer *> *stack = [NSMutableArray arrayWithObject:self.root];
    //
    //    while (stack.count > 0) {
    //        AFLayer *current = stack.lastObject;
    //        [stack removeLastObject];
    //
    //        if ([current.layerId isEqualToString:layerId]) {
    //            [self.layerMap setObject:current forKey:layerId];
    //            return current;
    //        }
    //
    //        for (AFLayer *child in current.children) {
    //            [stack addObject:child];
    //        }
    //    }

    return nil;
}

- (AFLayer *_Nullable)upgradeGroup:(NSArray<AFLayer *> *)layers {
    auto project = _venue->project();
    if (!project) {
        return nil;
    }

    if (layers.count <= 1) {
        return nil;
    }

    AFLayer *parent = layers[0].parent;
    for (AFLayer *layer : layers) {
        if (layer.parent != parent) {
            return nil;
        }
    }

    auto counter = project->genGroupCounter();
    AFLayer *group = [[AFLayer alloc] initWithName:[NSString stringWithFormat:@"Group %u", counter] type:AFLayerTypeGroup layerMap:self.layerMap];

    // 计算 group 的外包矩形
    CGFloat minX = CGFLOAT_MAX;
    CGFloat minY = CGFLOAT_MAX;
    CGFloat maxX = -CGFLOAT_MAX;
    CGFloat maxY = -CGFLOAT_MAX;

    for (AFLayer *layer in layers) {
        NSRect frame = layer.frame;
        minX = fmin(minX, frame.origin.x);
        minY = fmin(minY, frame.origin.y);
        maxX = fmax(maxX, frame.origin.x + frame.size.width);
        maxY = fmax(maxY, frame.origin.y + frame.size.height);
    }

    group.frame = NSMakeRect(minX, minY, maxX - minX, maxY - minY);

    [self.layerMap addLayer:group];

    auto index = [parent getChildIndex:layers.firstObject];
    // 将 group 添加到原父节点
    [parent addChild:group atIndex:index];

    // 添加子图层，并调整子图层 frame 相对于 group
    for (AFLayer *layer in layers) {
        // 调整 frame
        NSRect frame = layer.frame;
        frame.origin.x -= group.frame.origin.x;
        frame.origin.y -= group.frame.origin.y;
        layer.frame = frame;
        [layer removeFromParent];

        [group addChild:layer];
    }

    return group;
}

- (BOOL)undoGroup:(AFLayer *)group {
    if (group == nil) {
        return NO;
    }

    AFLayer *parent = group.parent;
    NSArray<AFLayer *> *children = group.children;
    if (parent == nil) {
        return NO;
    }

    auto index = [parent getChildIndex:group];
    [group removeFromParent];

    NSRect groupFrame = group.frame;
    for (AFLayer *child in children) {
        NSRect childFrame = child.frame;
        childFrame.origin.x += groupFrame.origin.x;
        childFrame.origin.y += groupFrame.origin.y;
        child.frame = childFrame;
        [parent addChild:child atIndex:index];
        index++;
    }
    return YES;
}

- (AFLayer *_Nullable)pickVenueAtUnderPoint:(NSPoint)point {
    //    auto root = _venue->rootLayer();
    auto container = _venue->containerLayer();
    //    auto local = container->globalToLocal(tgfx::Point{static_cast<float>(point.x), static_cast<float>(point.y)});
    auto layers = container->getLayersUnderPoint(static_cast<float>(point.x), static_cast<float>(point.y));
    if (layers.empty()) {
        return nil;
    }
    auto topLayer = layers.front();
    if (topLayer.get() == container) {
        return nil;
    }
    auto baseLayer = std::dynamic_pointer_cast<arenaforge::BaseLayer>(topLayer);
    if (!baseLayer) {
        return nil;
    }
    NSString *layerId = [NSString stringWithUTF8String:baseLayer->layerId().c_str()];
    AFLayer *layer = [self.layerMap getLayerById:layerId];
    AFLayer *parent = layer.parent;
    if (parent && parent.type == AFLayerTypeGroup) {
        layer = parent;
    }
    if (layer.parent == nil) {
        return nil;
    }
    return layer;
}

- (void)resetHoverWireframe {
    _venue->resetHoverWireframe();
}

- (BOOL)hitTestPoint:(NSPoint)point {
    auto root = _venue->rootLayerPtr();
    //    auto localPoint = root->globalToLocal(tgfx::Point{static_cast<float>(point.x), static_cast<float>(point.y)});
    //    auto container = _venue->container();
    auto hit = root->hitTestPoint(static_cast<float>(point.x), static_cast<float>(point.y));
    return hit;
}

- (NSPoint)globalToLocal:(NSPoint)point {
    auto root = _venue->rootLayer();
    auto local = root->globalToLocal(tgfx::Point{static_cast<float>(point.x), static_cast<float>(point.y)});
    return NSPointFromCGPoint(CGPointMake(local.x, local.y));
}

- (NSPoint)localToGlobal:(NSPoint)point {
    auto root = _venue->rootLayer();
    auto local = root->localToGlobal(tgfx::Point{static_cast<float>(point.x), static_cast<float>(point.y)});
    return NSPointFromCGPoint(CGPointMake(local.x, local.y));
}

#pragma mark - setter getter
- (NSString *)venueId {
    auto venueId = _venue->venueId();
    return [NSString stringWithUTF8String:venueId.c_str()];
}

- (NSString *)name {
    auto name = _venue->name();
    return [NSString stringWithUTF8String:name.c_str()];
}

- (void)setFrame:(NSRect)frame {
    auto cppRect = tgfx::Rect::MakeXYWH(
        static_cast<float>(frame.origin.x),
        static_cast<float>(frame.origin.y),
        static_cast<float>(frame.size.width),
        static_cast<float>(frame.size.height));
    _venue->setFrame(cppRect);
}

- (NSRect)frame {
    auto cppRect = _venue->frame();
    return NSRectFromCGRect(CGRectMake(cppRect.x(), cppRect.y(), cppRect.width(), cppRect.height()));
}

- (void)setBackgroundColor:(NSColor *)backgroundColor {
    if (backgroundColor == nil || backgroundColor == NSColor.clearColor) {
        _venue->setBackgroundColor(tgfx::Color::Transparent());
        return;
    }
    NSColor *rgbColor = [backgroundColor colorUsingColorSpace:[NSColorSpace sRGBColorSpace]];
    CGFloat red, green, blue, alpha;
    [rgbColor getRed:&red green:&green blue:&blue alpha:&alpha];
    auto cppColor = tgfx::Color{
        static_cast<float>(red),
        static_cast<float>(green),
        static_cast<float>(blue),
        static_cast<float>(alpha)};
    _venue->setBackgroundColor(cppColor);
}

- (NSColor *)backgroundColor {
    auto cppColor = _venue->backgroundColor();
    return [NSColor colorWithRed:cppColor.red green:cppColor.green blue:cppColor.blue alpha:cppColor.alpha];
}
@end
