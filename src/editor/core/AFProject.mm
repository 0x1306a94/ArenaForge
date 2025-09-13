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

#import "AFLayer+Private.h"
#import "AFProject+Private.h"
#import "AFVenue+Private.h"

#import "AFLayerMap.h"

@interface AFProject ()
@property (nonatomic, strong) NSURL *fileURL;
@property (nonatomic, strong) NSMutableDictionary<NSString *, AFVenue *> *venueMaps;
@property (nonatomic, strong) NSArray<AFVenue *> *orderVenues;
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
        project = arenaforge::Project::MakeFromJSONFile(rootDir + "/project.json", [=](const std::string &venueId) -> std::shared_ptr<arenaforge::Venue> {
            auto venueFilePath = rootDir + "/venues/" + venueId + ".json";
            return arenaforge::Venue::MakeFromJSONFile(venueFilePath);
        });
    } catch (const std::exception &e) {
        if (error) {
            NSString *msg = [NSString stringWithUTF8String:e.what()];
            *error = [NSError errorWithDomain:@"AFProject" code:400 userInfo:@{NSLocalizedFailureReasonErrorKey: msg}];
        }
        return nil;
    }

    if (self == [super init]) {
        self.fileURL = fileURL;

        _project = std::move(project);
        _venueMaps = [NSMutableDictionary<NSString *, AFVenue *> dictionary];

        NSMutableArray<AFVenue *> *orderVenues = [NSMutableArray<AFVenue *> array];
        for (auto cppObject : _project->venues()) {
            AFVenue *venue = [[AFVenue alloc] initWithCppObject:cppObject];
            _venueMaps[venue.venueId] = venue;
            [orderVenues addObject:venue];
        }
        _orderVenues = [orderVenues copy];
    }
    return self;
}

- (std::shared_ptr<arenaforge::Project>)cppObject {
    return _project;
}

- (void)rebuildOrderVenues {
    NSMutableArray<AFVenue *> *orderVenues = [NSMutableArray<AFVenue *> array];
    for (auto &item : _project->venues()) {
        NSString *venueId = [NSString stringWithUTF8String:item->venueId().c_str()];
        AFVenue *venue = self.venueMaps[venueId];
        if (venue) {
            [orderVenues addObject:venue];
        }
    }
    self.orderVenues = [orderVenues copy];
}

#pragma mark - public
- (NSString *)toJSONString {
    auto json = _project->toJSON();
    return [NSString stringWithUTF8String:json.c_str()];
}

- (AFVenue *)createVenue {
    auto counter = _project->genVenueCounter();
    AFVenue *venue = [[AFVenue alloc] initWithName:[NSString stringWithFormat:@"Venue %u", counter]];
    self.venueMaps[venue.venueId] = venue;
    return venue;
}

- (BOOL)addVenue:(AFVenue *)venue {
    if (venue == nil) {
        return NO;
    }
    auto index = _project->venues().size();
    return [self addVenue:venue atIndex:static_cast<int>(index)];
}

- (BOOL)addVenue:(AFVenue *)venue atIndex:(int)index {
    if (venue == nil) {
        return NO;
    }

    auto cppObject = [venue cppObject];
    if (!_project->addVenueAt(cppObject, index)) {
        return NO;
    }
    [self rebuildOrderVenues];
    if (self.venueChangeHandler) {
        self.venueChangeHandler(self);
    }
    return NO;
}

- (int)getVenueIndex:(AFVenue *)venue {
    if (venue == nil) {
        return -1;
    }

    auto cppObject = [venue cppObject];
    auto index = _project->getVeuneIndex(cppObject);
    return index;
}

- (void)removeVenue:(AFVenue *)venue {
    if (!venue) {
        return;
    }

    auto cppObject = [venue cppObject];
    if (!_project->removeVenue(cppObject)) {
        return;
    }
    [self rebuildOrderVenues];
    if (self.venueChangeHandler) {
        self.venueChangeHandler(self);
    }
}

- (AFVenue *_Nullable)pickVenueAtUnderPoint:(NSPoint)point {
    for (AFVenue *venue in self.orderVenues) {
        if ([venue hitTestPoint:point]) {
            return venue;
        }
    }
    return nil;
}

- (AFLayer *_Nullable)createLayerInVenue:(AFVenue *)venue {
    if (venue == nil) {
        return nil;
    }
    auto counter = _project->genRectangleCounter();
    AFLayer *layer = [[AFLayer alloc] initWithName:[NSString stringWithFormat:@"Rectangle %u", counter] type:AFLayerTypeRectangle layerMap:venue.layerMap];
    [venue.layerMap addLayer:layer];
    [venue.root addChild:layer];
    return layer;
}

- (AFLayer *_Nullable)createHoverWireframeLayerInVenue:(AFVenue *)venue targetLayer:(AFLayer *)targetLayer {
    if (venue == nil || targetLayer == nil) {
        return nil;
    }
    return [venue addHoverWireframeLayer:targetLayer];
}

#pragma mark - getter
- (NSArray<AFVenue *> *)venues {
    return self.orderVenues;
}
@end
