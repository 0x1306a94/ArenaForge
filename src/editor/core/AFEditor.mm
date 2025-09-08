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

#import <arenaforge/editor/core/AFEditor.h>

#import <arenaforge/editor/core/AFLayer.h>
#import <arenaforge/editor/core/AFProject.h>
#import <arenaforge/editor/core/Editor.h>

#import "AFEditor+Private.h"
#import "AFLayer+Private.h"
#import "AFProject+Private.h"

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

    auto project = [self.project cppProject];
    _editor = arenaforge::editor::Editor::Make(std::move(project));
}

- (void)setupCanvasView:(AFMacCanvasView *)canvasView {
    if (_canvasView == canvasView) {
        return;
    }

    _canvasView.delegate = nil;
    [_canvasView setupEditor:nullptr];
    self.canvasView = canvasView;

    if (_editor != nullptr) {
        if (canvasView == nil) {
            _editor->setRendererBackend(nullptr);
        } else {
            [canvasView setupEditor:_editor];
            canvasView.delegate = self;
            auto rendererBackend = std::make_shared<arenaforge::editor::MacRendererBackend>((NSView *)canvasView);
            _editor->setRendererBackend(std::move(rendererBackend));
        }
    }
}

- (AFLayer *_Nullable)createLayerWithName:(NSString *)name {
    return [AFLayer createWithName:name];
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
