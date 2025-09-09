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
//  AFEditor.mm
//  arenaforge
//
//  Created by KK on 2025/9/6.
//

#import <arenaforge_editor/core/AFEditor.h>

#import <arenaforge_editor/core/AFLayer.h>
#import <arenaforge_editor/core/AFProject.h>
#import <arenaforge_editor/core/AFVenue.h>
#import <arenaforge_editor/core/Editor.h>

#import "AFEditor+Private.h"
#import "AFLayer+Private.h"
#import "AFProject+Private.h"
#import "AFVenue+Private.h"

#import "platform/mac/AFMacCanvasView+Private.h"
#import "platform/mac/MacRendererBackend.h"

@interface AFEditor () <AFMacCanvasViewDelegate>
@property (nonatomic, strong) AFProject *project;
@property (nonatomic, strong) AFMacCanvasView *canvasView;
@end

@implementation AFEditor {
    std::shared_ptr<arenaforge::editor::Editor> _editor;
}

#if DEBUG
- (void)dealloc {
    NSLog(@"[%@ dealloc]", NSStringFromClass(self.class));
}
#endif

- (instancetype)initWithProject:(AFProject *)project {
    if (self == [super init]) {
        self.project = project;

        [self commonInit];
    }
    return self;
}

- (void)commonInit {
    auto cppProject = [self.project cppObject];
    _editor = arenaforge::editor::Editor::Make(std::move(cppProject));

    [self onVenueChanges];

    __weak AFEditor *weakSelf = self;
    self.project.venueChangeHandler = ^(AFProject *_Nonnull project) {
        UNUSED_PARAM(project);
        if (weakSelf == nil) {
            return;
        }
        [weakSelf onVenueChanges];
    };
}

- (void)onVenueChanges {
    _editor->onVenueChanges();
}
#pragma mark - public

- (void)setupCanvasView:(AFMacCanvasView *)canvasView {
    if (_canvasView == canvasView) {
        return;
    }

    _canvasView.delegate = nil;
    self.canvasView = canvasView;

    if (_editor != nullptr) {
        if (canvasView == nil) {
            _editor->setRendererBackend(nullptr);
        } else {
            canvasView.delegate = self;
            auto rendererBackend = std::make_shared<arenaforge::editor::MacRendererBackend>((NSView *)canvasView);
            _editor->setRendererBackend(std::move(rendererBackend));
        }
    }
}

- (CGFloat)zoomScale {
    if (_editor) {
        return _editor->zoomScale();
    }
    return 1.0;
}

- (NSPoint)contentOffset {
    float offsetX = 0, offsetY = 0;
    if (_editor) {
        _editor->contentOffset(offsetX, offsetY);
    }
    return NSPointFromCGPoint(CGPointMake(offsetX, offsetY));
}

- (CGFloat)density {
    if (_editor) {
        return _editor->density();
    }
    return 1.0;
}

- (void)updateZoomScale:(CGFloat)zoomScale {
    if (_editor) {
        float offsetX = 0, offsetY = 0;
        _editor->contentOffset(offsetX, offsetY);
        _editor->updateZoomAndOffset(static_cast<float>(zoomScale), offsetX, offsetY);
    }
}

- (void)updateOffset:(NSPoint)offset {
    if (_editor) {
        auto zoomScale = _editor->zoomScale();
        _editor->updateZoomAndOffset(zoomScale, static_cast<float>(offset.x), static_cast<float>(offset.y));
    }
}

- (void)updateZoomScale:(CGFloat)zoomScale offset:(NSPoint)offset {
    if (_editor) {
        _editor->updateZoomAndOffset(static_cast<float>(zoomScale), static_cast<float>(offset.x), static_cast<float>(offset.y));
    }
}

#pragma mark - AFMacCanvasViewDelegate
- (void)AFMacCanvasViewDidUpdateSize:(AFMacCanvasView *)view {
    if (_editor) {
        _editor->updateSize();
    }
}

- (void)AFMacCanvasViewDidDraw:(AFMacCanvasView *)view {
    if (_editor) {
        _editor->draw();
    }
}
@end
