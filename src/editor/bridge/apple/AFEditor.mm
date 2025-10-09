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

#import <arenaforge_editor/bridge/apple/AFEditor.h>

#import <arenaforge_editor/bridge/apple/AFLayer.h>
#import <arenaforge_editor/bridge/apple/AFProject.h>
#import <arenaforge_editor/core/Editor.h>
#import <arenaforge_editor/core/HoverManager.h>
#import <arenaforge_editor/core/SelectionManager.h>

#import "AFEditor+Private.h"
#import "AFLayer+Private.h"
#import "AFLayerMap.h"
#import "AFProject+Private.h"

#import "renderer/LayerTreeAdapter.h"

#import "platform/mac/AFMacCanvasView+Private.h"
#import "platform/mac/MacRendererBackend.h"

#import <tgfx/layers/ShapeLayer.h>

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

- (CGPoint)contentOffset {
    arenaforge::Point offset;
    if (_editor) {
        offset = _editor->contentOffset();
    }
    return CGPointMake(offset.x, offset.y);
}

- (CGFloat)density {
    if (_editor) {
        return _editor->density();
    }
    return 1.0;
}

- (CGPoint)toCanvasPoint:(CGPoint)source {
    CGFloat currentZoom = [self zoomScale];
    CGFloat density = [self density];
    CGPoint contentOffset = [self contentOffset];

    CGFloat px = source.x * density;
    CGFloat py = source.y * density;
    CGFloat x = (px - contentOffset.x) / currentZoom;
    CGFloat y = (py - contentOffset.y) / currentZoom;

    return CGPointMake(x, y);
}

- (void)autoAdjustCanvasScaleForContent {
    if (_editor) {
        _editor->autoAdjustCanvasScaleForContent();
    }
}

- (void)updateZoomScale:(CGFloat)zoomScale {
    if (_editor) {
        auto contentOffset = _editor->contentOffset();
        _editor->updateZoomAndOffset(static_cast<float>(zoomScale), contentOffset.x, contentOffset.y);
    }
}

- (void)updateOffset:(CGPoint)offset {
    if (_editor) {
        auto zoomScale = _editor->zoomScale();
        _editor->updateZoomAndOffset(zoomScale, static_cast<float>(offset.x), static_cast<float>(offset.y));
    }
}

- (void)updateZoomScale:(CGFloat)zoomScale offset:(CGPoint)offset {
    if (_editor) {
        _editor->updateZoomAndOffset(static_cast<float>(zoomScale), static_cast<float>(offset.x), static_cast<float>(offset.y));
    }
}

- (BOOL)addLayer:(AFLayer *)layer toParent:(AFLayer *)parent {
    if (!_editor) {
        return NO;
    }

    if (layer == nil || parent == nil) {
        return NO;
    }

    return [parent addChild:layer];
}

- (AFLayer *_Nullable)findLayerAtPoint:(CGPoint)point {
    auto cppLayer = _editor->findLayerAtPoint(static_cast<float>(point.x), static_cast<float>(point.y), true);
    if (!cppLayer) {
        if (_editor->hitTestPointInContainer(static_cast<float>(point.x), static_cast<float>(point.y))) {
            return self.project.root;
        }
        return nil;
    }

    auto cLayerId = cppLayer->layerId();
    NSString *layerId = [NSString stringWithUTF8String:cLayerId.c_str()];
    AFLayer *layer = [self.project.layerMap getLayerById:layerId];
    if (layer.parent && layer.parent.type == AFLayerTypeGroup && !layer.parent.isRoot) {
        return layer.parent;
    }
    return layer;
}

- (CGPoint)globalToLocal:(CGPoint)point targetLayer:(AFLayer *)layer {
    if (!_editor || layer == nil) {
        return point;
    }
    auto cppLayer = [layer cppObject];
    auto result = _editor->globalToLocal({static_cast<float>(point.x), static_cast<float>(point.y)}, cppLayer);
    if (!result) {
        return point;
    }
    auto &local = result.value();
    return CGPointMake(local.x, local.y);
}

- (CGPoint)localToGlobal:(CGPoint)point sourceLayer:(AFLayer *)layer {
    if (!_editor || layer == nil) {
        return point;
    }

    auto cppLayer = [layer cppObject];
    auto result = _editor->localToGlobal({static_cast<float>(point.x), static_cast<float>(point.y)}, cppLayer);
    if (!result) {
        return point;
    }
    auto &global = result.value();
    return CGPointMake(global.x, global.y);
}

- (void)createHoverWireframeLayerInTargetLayer:(AFLayer *)targetLayer {
    if (targetLayer == nil) {
        return;
    }
    auto cppLayer = [targetLayer cppObject];
    if (!cppLayer) {
        return;
    }

    if (!_editor) {
        return;
    }
    auto hoverManager = _editor->hoverManager();
    hoverManager->addHoverWireframe({cppLayer});
}

- (void)resetHoverWireframe {
    if (!_editor) {
        return;
    }
    auto hoverManager = _editor->hoverManager();
    hoverManager->resetHoverWireframe();
}

- (void)selectLayers:(NSArray<AFLayer *> *)layers {
    if (!_editor) {
        return;
    }
    auto selectionManager = _editor->selectionManager();

    std::vector<std::shared_ptr<arenaforge::Layer>> targets{};
    for (AFLayer *layer : layers) {
        auto cppObject = [layer cppObject];
        targets.push_back(cppObject);
    }

    selectionManager->updateSelection(targets);
}

- (void)beginSelectLayerMove:(CGPoint)point {
    if (!_editor) {
        return;
    }
    auto selectionManager = _editor->selectionManager();
    selectionManager->beginMove(static_cast<float>(point.x), static_cast<float>(point.y));
}

- (void)updateSelectLayerMove:(CGPoint)point {
    if (!_editor) {
        return;
    }
    auto selectionManager = _editor->selectionManager();
    selectionManager->updateMove(static_cast<float>(point.x), static_cast<float>(point.y));
}

- (void)endSelectLayerMove {
    if (!_editor) {
        return;
    }
    auto selectionManager = _editor->selectionManager();
    selectionManager->endMove();
}

- (void)clearSelection {
    if (!_editor) {
        return;
    }
    auto selectionManager = _editor->selectionManager();
    selectionManager->clearSelection();
}

- (void)updateSelectionDisplay {
    if (!_editor) {
        return;
    }
    auto selectionManager = _editor->selectionManager();
    selectionManager->updateSelectionDisplay();
}

- (void)clearSelectionDisplay {
    if (!_editor) {
        return;
    }
    auto selectionManager = _editor->selectionManager();
    selectionManager->clearSelectionDisplay();
}

- (bool)hitTestPointInSelectedBoundingBox:(CGPoint)point {
    if (!_editor) {
        return false;
    }
    auto selectionManager = _editor->selectionManager();
    auto hit = selectionManager->hitTestPointInSelectedBoundingBox(static_cast<float>(point.x), static_cast<float>(point.y));
    return hit;
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
