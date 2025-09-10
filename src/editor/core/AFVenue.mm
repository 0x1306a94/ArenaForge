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

#import <arenaforge_core/Venue.h>
#import <arenaforge_core/uuid/UUID.h>

#import "AFLayer+Private.h"

#import "AFVenue+Private.h"

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
        auto uuid = arenaforge::UUID::Instance();
        auto venueId = uuid();
        _venue = arenaforge::Venue::Make(venueId, (name == nil ? "" : std::string(name.UTF8String)), "");
        _venue->setBackgroundColor(tgfx::Color::FromRGBA(0xcc, 0xcc, 0xcc));

        _root = [[AFLayer alloc] initWithCppObject:_venue->containerPtr()];
    }
    return self;
}

- (instancetype)initWithCppObject:(std::shared_ptr<arenaforge::Venue>)cppObject {
    if (self == [super init]) {
        _venue = std::move(cppObject);

        _root = [[AFLayer alloc] initWithCppObject:_venue->containerPtr()];
    }
    return self;
}

- (std::shared_ptr<arenaforge::Venue>)cppObject {
    return _venue;
}

- (NSString *)toJSONString {
    auto json = _venue->toJSON();
    return [NSString stringWithUTF8String:json.c_str()];
}

- (AFLayer *)createLayerWithName:(NSString *)name {
    AFLayer *layer = [[AFLayer alloc] initWithName:name];
    auto container = _venue->container();
    auto cppLayer = [layer cppObject];
    container->addChild(cppLayer);
    return layer;
}

- (BOOL)hitTestPoint:(NSPoint)point {
    auto root = _venue->rootPtr();
    //    auto localPoint = root->globalToLocal(tgfx::Point{static_cast<float>(point.x), static_cast<float>(point.y)});
    //    auto container = _venue->container();
    auto hit = root->hitTestPoint(static_cast<float>(point.x), static_cast<float>(point.y));
    return hit;
}

- (NSPoint)globalToLocal:(NSPoint)point {
    auto root = _venue->root();
    auto local = root->globalToLocal(tgfx::Point{static_cast<float>(point.x), static_cast<float>(point.y)});
    return NSPointFromCGPoint(CGPointMake(local.x, local.y));
}

- (NSPoint)localToGlobal:(NSPoint)point {
    auto root = _venue->root();
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
    if (backgroundColor == nil) {
        _venue->setBackgroundColor(tgfx::Color::Transparent());
        return;
    }
    CGFloat red, green, blue, alpha;
    [backgroundColor getRed:&red green:&green blue:&blue alpha:&alpha];
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
