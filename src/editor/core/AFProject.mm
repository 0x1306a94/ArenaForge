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

#import <arenaforge_editor/core/AFProject.h>

#import <arenaforge_editor/core/AFVenue.h>

#import <arenaforge_editor/core/AFShapeLayer.h>

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

- (AFShapeLayer *_Nullable)createShapeLayer {
    AFShapeLayer *layer = [[AFShapeLayer alloc] initWithName:@"Shape" layerMap:self.layerMap];
    return layer;
}

- (AFLayer *_Nullable)findLayerById:(NSString *)layerId {
    if (layerId == nil) {
        return nil;
    }
    return nil;
}

- (AFLayer *_Nullable)pickLayerAtUnderPoint:(NSPoint)point {
    //    auto root = _venue->rootLayer();
    //    auto container = _venue->containerLayer();
    //        //    auto local = container->globalToLocal(tgfx::Point{static_cast<float>(point.x), static_cast<float>(point.y)});
    //    auto layers = container->getLayersUnderPoint(static_cast<float>(point.x), static_cast<float>(point.y));
    //    if (layers.empty()) {
    //        return nil;
    //    }
    //    auto topLayer = layers.front();
    //    if (topLayer.get() == container) {
    //        return nil;
    //    }
    //    auto baseLayer = std::dynamic_pointer_cast<arenaforge::BaseLayer>(topLayer);
    //    if (!baseLayer) {
    //        return nil;
    //    }
    //    NSString *layerId = [NSString stringWithUTF8String:baseLayer->layerId().c_str()];
    //    AFLayer *layer = [self.layerMap getLayerById:layerId];
    //    AFLayer *parent = layer.parent;
    //    if (parent && parent.type == AFLayerTypeGroup) {
    //        layer = parent;
    //    }
    //    if (layer.parent == nil) {
    //        return nil;
    //    }
    return nil;
}

- (BOOL)hitTestPoint:(NSPoint)point {
    //    auto root = _venue->rootLayerPtr();
    //        //    auto localPoint = root->globalToLocal(tgfx::Point{static_cast<float>(point.x), static_cast<float>(point.y)});
    //        //    auto container = _venue->container();
    //    auto hit = root->hitTestPoint(static_cast<float>(point.x), static_cast<float>(point.y));
    //    return hit;
    return point.x > 0;
}

- (NSPoint)globalToLocal:(NSPoint)point {
    //    auto root = _venue->rootLayer();
    //    auto local = root->globalToLocal(tgfx::Point{static_cast<float>(point.x), static_cast<float>(point.y)});
    //    return NSPointFromCGPoint(CGPointMake(local.x, local.y));
    return point;
}

- (NSPoint)localToGlobal:(NSPoint)point {
    //    auto root = _venue->rootLayer();
    //    auto global = root->localToGlobal(tgfx::Point{static_cast<float>(point.x), static_cast<float>(point.y)});
    //    return NSPointFromCGPoint(CGPointMake(global.x, global.y));
    return point;
}

- (AFLayer *_Nullable)createHoverWireframeLayerInTargetLayer:(AFLayer *)targetLayer {
    if (targetLayer == nil) {
        return nil;
    }
    return nil;
}

- (void)resetHoverWireframe {
}

#pragma mark - getter

@end
