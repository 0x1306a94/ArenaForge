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
//  AFProject.mm
//  arenaforge
//
//  Created by KK on 2025/9/7.
//

#import <arenaforge_editor/bridge/apple/AFProject.h>

#import <arenaforge_core/PathCommand.h>
#import <arenaforge_core/layers/ShapeLayer.h>
#import <arenaforge_editor/bridge/apple/AFShapeLayer.h>

#import "AFLayer+Private.h"
#import "AFProject+Private.h"

#import "AFLayerMap.h"

@interface AFProject ()
@property (nonatomic, strong) NSURL *fileURL;
@end

@implementation AFProject {
    std::shared_ptr<arenaforge::Project> _project;
}
#if DEBUG
- (void)dealloc {
    NSLog(@"[%@ dealloc]", NSStringFromClass(self.class));
}
#endif

- (instancetype)initWithName:(NSString *)name description:(NSString *)description width:(CGFloat)width height:(CGFloat)height {
    if (self == [super init]) {
        _layerMap = [AFLayerMap new];
        _project = arenaforge::Project::Make((name != nil ? name.UTF8String : ""), (description != nil ? description.UTF8String : ""), {static_cast<float>(width), static_cast<float>(height)});
        auto rootCpp = _project->root();
        _root = [[AFLayer alloc] initWithCppObject:std::move(rootCpp) layerMap:_layerMap];
        [_layerMap addLayer:_root];
    }
    return self;
}

- (instancetype __nullable)initWithFileURL:(NSURL *)fileURL error:(NSError **)error {

    std::shared_ptr<arenaforge::Project> project;
    try {
        auto rootDir = std::string(fileURL.path.UTF8String);
        project = arenaforge::Project::MakeFromJSONFile(rootDir);
    } catch (const std::exception &e) {
        if (error) {
            NSString *msg = [NSString stringWithUTF8String:e.what()];
            *error = [NSError errorWithDomain:@"AFProject" code:400 userInfo:@{NSLocalizedFailureReasonErrorKey: msg}];
        }
        return nil;
    }

    if (self == [super init]) {
        self.fileURL = fileURL;
        _layerMap = [AFLayerMap new];
        _project = std::move(project);

        auto rootCpp = _project->root();
        _root = [[AFLayer alloc] initWithCppObject:std::move(rootCpp) layerMap:_layerMap];
        [_layerMap addLayer:_root];
    }
    return self;
}

- (std::shared_ptr<arenaforge::Project>)cppObject {
    return _project;
}

#pragma mark - public
- (NSString *)toJSONString {
    auto json = _project->toJSON();
    return [NSString stringWithUTF8String:json.c_str()];
}

- (AFLayer *_Nullable)createLayer {
    return nil;
}

- (AFShapeLayer *_Nullable)createShapeLayer:(AFBuiltinShapeType)type {
    if (type < AFBuiltinShapeTypeRectangle || type > AFBuiltinShapeTypeTriangle) {
        return nil;
    }
    auto typeName = AFBuiltinShapeTypeToString(type);
    auto counter = _project->genRectangleCounter();
    NSString *name = [NSString stringWithFormat:@"%@ %u", typeName, counter];
    AFShapeLayer *layer = [[AFShapeLayer alloc] initWithName:name layerMap:self.layerMap];
    auto cppLayer = std::static_pointer_cast<arenaforge::ShapeLayer>([layer cppObject]);
    auto commands = arenaforge::PathCommand::MakeFrom(static_cast<arenaforge::BuiltinShapeType>(type));
    cppLayer->setPathCommands(std::move(commands));
    [self.layerMap addLayer:layer];
    return layer;
}

- (AFLayer *_Nullable)findLayerById:(NSString *)layerId {
    if (layerId == nil) {
        return nil;
    }
    AFLayer *layer = [self.layerMap getLayerById:layerId];
    return layer;
}

- (AFLayer *_Nullable)upgradeGroup:(NSArray<AFLayer *> *)layers {
    if (layers.count <= 1) {
        return nil;
    }

    AFLayer *parent = layers[0].parent;
    for (AFLayer *layer : layers) {
        if (layer.parent != parent) {
            return nil;
        }
    }

    auto counter = _project->genGroupCounter();
    auto name = [NSString stringWithFormat:@"Group %u", counter];
    AFLayer *group = [[AFLayer alloc] initWithName:name layerMap:self.layerMap];
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

    NSArray<AFLayer *> *sortedLayers = [layers sortedArrayUsingComparator:^NSComparisonResult(AFLayer *_Nonnull obj1, AFLayer *_Nonnull obj2) {
        int lhsIndex = [parent getChildIndex:obj1];
        int rhsIndex = [parent getChildIndex:obj2];
        if (lhsIndex < rhsIndex) {
            return NSOrderedAscending;
        } else if (lhsIndex > rhsIndex) {
            return NSOrderedDescending;
        } else {
            return NSOrderedSame;
        }
    }];

    auto insetIndex = [parent getChildIndex:sortedLayers.firstObject];
    // 将 group 添加到原父节点
    [parent addChild:group atIndex:insetIndex];

    // 添加子图层，并调整子图层 frame 相对于 group
    for (AFLayer *layer in sortedLayers) {
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

#pragma mark - getter

@end
