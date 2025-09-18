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

#import <arenaforge_core/layers/Layer.h>
#import <arenaforge_core/uuid/UUID.h>

#include <tgfx/layers/SolidColor.h>

#import "AFLayer+Private.h"
#import "AFLayerMap.h"

@interface AFLayer ()
@property (nonatomic, weak) AFLayer *parent;
@property (nonatomic, weak) AFLayerMap *layerMap;
@property (nonatomic, strong) NSArray<AFLayer *> *cacheChildren;
@end

@implementation AFLayer {
    std::shared_ptr<arenaforge::Layer> _layer;
}

//#if DEBUG
//- (void)dealloc {
//    NSLog(@"[%@ dealloc]", NSStringFromClass(self.class));
//}
//#endif

- (instancetype)initWithName:(NSString *)name layerMap:(AFLayerMap *)layerMap {
    if (self == [super init]) {
        _layerMap = layerMap;
        _layer = [self createCppObject:name];

        [self rebuildCacheChildren];
    }
    return self;
}

- (instancetype)initWithCppObject:(std::shared_ptr<arenaforge::Layer>)cppObject layerMap:(AFLayerMap *)layerMap {
    if (self == [super init]) {
        _layer = std::move(cppObject);
        _layerMap = layerMap;
        for (auto &cppChild : _layer->children()) {
            AFLayer *child = [[AFLayer alloc] initWithCppObject:std::static_pointer_cast<arenaforge::Layer>(cppChild) layerMap:layerMap];
            child.parent = self;
            [layerMap addLayer:child];
        }

        [self rebuildCacheChildren];
    }
    return self;
}

- (std::shared_ptr<arenaforge::Layer>)createCppObject:(NSString *_Nullable)name {
    auto uuid = arenaforge::UUID::Instance();
    auto layerId = uuid();
    auto layer = arenaforge::Layer::Make(layerId);
    layer->setName((name == nil ? "" : std::string(name.UTF8String)));
    return layer;
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

- (void)rebuildCacheChildren {
    NSMutableArray<AFLayer *> *children = [NSMutableArray<AFLayer *> array];
    for (const auto &child : _layer->children()) {
        NSString *layerId = [NSString stringWithUTF8String:child->layerId().c_str()];
        AFLayer *layer = [self.layerMap getLayerById:layerId];
        if (layer) {
            [children addObject:layer];
        }
    }
    self.cacheChildren = [children copy];
}
#pragma mark - setter getter

- (std::shared_ptr<arenaforge::Layer>)cppObject {
    return _layer;
}

- (NSString *)layerId {
    auto layerId = _layer->layerId();
    return [NSString stringWithUTF8String:layerId.c_str()];
}

- (AFLayerType)type {
    auto type = static_cast<AFLayerType>(_layer->type());
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
    auto cppRect = arenaforge::Rect::MakeXYWH(
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

- (BOOL)isRoot {
    return _layer->isRoot();
}

@end
