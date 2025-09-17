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
//  AFShapeLayer.mm
//  arenaforge_editor
//
//  Created by king on 2025/9/17.
//

#include <arenaforge_editor/core/AFShapeLayer.h>

#include <arenaforge_core/layers/ShapeLayer.h>
#import <arenaforge_core/uuid/UUID.h>

#import <AppKit/NSColorSpace.h>

#import "AFLayer+Private.h"

@implementation AFShapeLayer
- (std::shared_ptr<arenaforge::Layer>)createCppObject:(NSString *_Nullable)name {
    auto uuid = arenaforge::UUID::Instance();
    auto layerId = uuid();
    auto layer = arenaforge::ShapeLayer::Make(layerId);
    layer->setName((name == nil ? "" : std::string(name.UTF8String)));
    return layer;
}

- (void)setFillColor:(NSColor *_Nullable)fillColor {
    auto cppObject = std::static_pointer_cast<arenaforge::ShapeLayer>([self cppObject]);
    if (fillColor == nil) {
        cppObject->setFill(std::nullopt);
        return;
    }

    NSColor *rgbColor = [fillColor colorUsingColorSpace:[NSColorSpace sRGBColorSpace]];
    CGFloat red, green, blue, alpha;
    [rgbColor getRed:&red green:&green blue:&blue alpha:&alpha];
    auto cppColor = arenaforge::Color{
        static_cast<float>(red),
        static_cast<float>(green),
        static_cast<float>(blue),
        static_cast<float>(alpha)};

    cppObject->setFill(std::move(cppColor));
}

- (NSColor *_Nullable)fillColor {
    auto cppObject = std::static_pointer_cast<arenaforge::ShapeLayer>([self cppObject]);
    auto fill = cppObject->fill();
    if (!fill) {
        return nil;
    }
    auto cppColor = fill.value();
    if (cppColor == arenaforge::Color::Transparent()) {
        return NSColor.clearColor;
    }
    return [NSColor colorWithRed:cppColor.red green:cppColor.green blue:cppColor.blue alpha:cppColor.alpha];
}

- (void)setStrokeColor:(NSColor *_Nullable)strokeColor {
    auto cppObject = std::static_pointer_cast<arenaforge::ShapeLayer>([self cppObject]);
    if (strokeColor == nil) {
        cppObject->setStroke(std::nullopt);
        return;
    }

    NSColor *rgbColor = [strokeColor colorUsingColorSpace:[NSColorSpace sRGBColorSpace]];
    CGFloat red, green, blue, alpha;
    [rgbColor getRed:&red green:&green blue:&blue alpha:&alpha];
    auto cppColor = arenaforge::Color{
        static_cast<float>(red),
        static_cast<float>(green),
        static_cast<float>(blue),
        static_cast<float>(alpha)};

    cppObject->setStroke(std::move(cppColor));
}

- (NSColor *_Nullable)strokeColor {
    auto cppObject = std::static_pointer_cast<arenaforge::ShapeLayer>([self cppObject]);
    auto stroke = cppObject->stroke();
    if (!stroke) {
        return nil;
    }
    auto cppColor = stroke.value();
    if (cppColor == arenaforge::Color::Transparent()) {
        return NSColor.clearColor;
    }
    return [NSColor colorWithRed:cppColor.red green:cppColor.green blue:cppColor.blue alpha:cppColor.alpha];
}

- (void)setLineWidth:(CGFloat)lineWidth {
    auto cppObject = std::static_pointer_cast<arenaforge::ShapeLayer>([self cppObject]);
    cppObject->setLineWidth(static_cast<float>(lineWidth));
}

- (CGFloat)lineWidth {
    auto cppObject = std::static_pointer_cast<arenaforge::ShapeLayer>([self cppObject]);
    return static_cast<CGFloat>(cppObject->lineWidth());
}

@end
