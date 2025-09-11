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

@interface AFProject ()
@property (nonatomic, strong) NSURL *fileURL;
@property (nonatomic, strong) NSMutableArray<AFVenue *> *internalVenues;
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
        _internalVenues = [NSMutableArray<AFVenue *> array];

        for (auto cppObject : _project->venues()) {
            AFVenue *venue = [[AFVenue alloc] initWithCppObject:cppObject];
            [_internalVenues addObject:venue];
        }
    }
    return self;
}

- (std::shared_ptr<arenaforge::Project>)cppObject {
    return _project;
}

- (NSString *)toJSONString {
    auto json = _project->toJSON();
    return [NSString stringWithUTF8String:json.c_str()];
}

- (AFVenue *)createVenue {
    auto counter = _project->genVenueCounter();
    AFVenue *venue = [[AFVenue alloc] initWithName:[NSString stringWithFormat:@"Venue %u", counter]];
    auto cppVenue = [venue cppObject];
    _project->addVenue(cppVenue);
    [self.internalVenues addObject:venue];
    if (self.venueChangeHandler) {
        self.venueChangeHandler(self);
    }
    return venue;
}

- (void)removeVenue:(AFVenue *)venue {
    if (!venue) {
        return;
    }

    if ([self.internalVenues containsObject:venue]) {
        auto cppVenue = [venue cppObject];
        _project->removeVenue(std::move(cppVenue));
        [self.internalVenues removeObject:venue];
        if (self.venueChangeHandler) {
            self.venueChangeHandler(self);
        }
    }
}

- (AFVenue *_Nullable)pickVenueAtUnderPoint:(NSPoint)point {
    for (AFVenue *venue in self.internalVenues) {
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
    AFLayer *layer = [[AFLayer alloc] initWithName:[NSString stringWithFormat:@"Rectangle %u", counter]];
    [venue.root addChild:layer];
    return layer;
}

#pragma mark - getter
- (NSArray<AFVenue *> *)venues {
    return [self.internalVenues copy];
}
@end
