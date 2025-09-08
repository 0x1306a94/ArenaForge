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
//  AFMacCanvasView.mm
//  arenaforge_editor
//
//  Created by KK on 2025/9/6.
//

#import <arenaforge/editor/platform/mac/AFMacCanvasView.h>

#import <QuartzCore/CADisplayLink.h>

#import "AFMacCanvasView+Private.h"

#import <tgfx/core/Point.h>

@interface AFMacCanvasView ()
@property (nonatomic, assign) CVDisplayLinkRef cvDisplayLink;
@property (nonatomic, strong) CADisplayLink *caDisplayLink;
@property (nonatomic, strong) NSTrackingArea *trackingArea;
- (void)updateSize;
- (void)draw;
@end

static CVReturn OnDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *, CVOptionFlags, CVOptionFlags *, void *userInfo) {
    AFMacCanvasView *view = (__bridge AFMacCanvasView *)userInfo;
    [view draw];
    return kCVReturnSuccess;
}

@implementation AFMacCanvasView {
    std::weak_ptr<arenaforge::editor::Editor> _editor;
}

- (void)dealloc {
    [self clearDisplayLink];
#if DEBUG
    NSLog(@"[%@ dealloc]", NSStringFromClass(self.class));
#endif
}

- (BOOL)acceptsFirstResponder {
    return YES;
}

- (void)setBounds:(CGRect)bounds {
    CGRect oldBounds = self.bounds;
    [super setBounds:bounds];
    if (oldBounds.size.width != bounds.size.width || oldBounds.size.height != bounds.size.height) {
        [self updateSize];
    }
}

- (void)setFrame:(CGRect)frame {
    CGRect oldRect = self.frame;
    [super setFrame:frame];
    if (oldRect.size.width != frame.size.width || oldRect.size.height != frame.size.height) {
        [self updateSize];
    }
}

- (void)viewDidMoveToWindow {
    [super viewDidMoveToWindow];

    if (self.window) {
        [self.window makeFirstResponder:self];
        [self updateSize];
        [self setupDisplayLink];
        [self startDisplayLink];
    } else {
        [self clearDisplayLink];
    }
}

- (BOOL)isFlipped {
    return YES;
}

- (void)updateSize {
    [self.delegate AFMacCanvasViewDidUpdateSize:self];

    [self updateTrackingArea];
}

- (void)draw {
    [self.delegate AFMacCanvasViewDidDraw:self];
}

- (void)updateTrackingArea {
    NSRect bounds = self.bounds;
    if (self.trackingArea) {
        [self removeTrackingArea:self.trackingArea];
        self.trackingArea = nil;
    }

    if (NSIsEmptyRect(bounds)) {
        return;
    }

    NSTrackingAreaOptions options = NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveAlways;
    NSTrackingArea *trackingArea = [[NSTrackingArea alloc] initWithRect:bounds options:options owner:self userInfo:nil];
    self.trackingArea = trackingArea;
    [self addTrackingArea:trackingArea];
}

- (void)clearDisplayLink {
    [self stopDisplayLink];
    if (@available(macOS 14, *)) {
        if (self.caDisplayLink) {
            [self.caDisplayLink invalidate];
            self.caDisplayLink = nil;
        }
    } else {
        if (self.cvDisplayLink) {
            CVDisplayLinkStop(self.cvDisplayLink);
            CVDisplayLinkRelease(self.cvDisplayLink);
            self.cvDisplayLink = nil;
        }
    }
}

- (void)setupDisplayLink {
    if (@available(macOS 14, *)) {
        if (self.caDisplayLink) {
            return;
        }
        self.caDisplayLink = [self displayLinkWithTarget:self selector:@selector(draw)];
        [self.caDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
    } else {
        if (self.cvDisplayLink) {
            return;
        }
        CVDisplayLinkCreateWithActiveCGDisplays(&_cvDisplayLink);
        CVDisplayLinkSetOutputCallback(_cvDisplayLink, &OnDisplayLinkCallback, (__bridge void *)self);
    }
}

- (void)startDisplayLink {
    if (@available(macOS 14, *)) {
        self.caDisplayLink.paused = NO;
    } else {
        if (self.cvDisplayLink) {
            CVDisplayLinkStart(self.cvDisplayLink);
        }
    }
}

- (void)stopDisplayLink {
    if (@available(macOS 14, *)) {
        self.caDisplayLink.paused = YES;
    } else {
        if (self.cvDisplayLink) {
            CVDisplayLinkStop(self.cvDisplayLink);
        }
    }
}

- (void)setupEditor:(std::shared_ptr<arenaforge::editor::Editor>)editor {
    if (editor) {
        _editor = std::move(editor);
    } else {
        _editor.reset();
    }
}

- (tgfx::Point)toCanvasPoint:(tgfx::Point)source editor:(const arenaforge::editor::Editor *)editor {
    if (editor == nullptr) {
        return source;
    }

    auto currentZoom = editor->zoomScale();
    auto density = editor->density();
    float offsetX = 0, offsetY = 0;
    if (!editor->contentOffset(offsetX, offsetY)) {
        return source;
    }

    float px = static_cast<float>(source.x * density);
    float py = static_cast<float>(source.y * density);

    float x = (px - offsetX) / currentZoom;
    float y = (py - offsetY) / currentZoom;

    return tgfx::Point::Make(x, y);
}

- (void)mouseDown:(NSEvent *)event {
    if (_editor.expired()) {
        return;
    }
    auto editor = _editor.lock();
    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    auto canvasLocation = [self toCanvasPoint:tgfx::Point::Make(static_cast<float>(location.x), static_cast<float>(location.y)) editor:editor.get()];

    NSLog(@"mouseDown location: {%.f, %.f} canvasLocation: {%.f, %.f}", location.x, location.y, canvasLocation.x, canvasLocation.y);
}

- (void)mouseMoved:(NSEvent *)event {
    if (_editor.expired()) {
        return;
    }
    auto editor = _editor.lock();
    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    auto canvasLocation = [self toCanvasPoint:tgfx::Point::Make(static_cast<float>(location.x), static_cast<float>(location.y)) editor:editor.get()];

    NSLog(@"mouseMoved location: {%.f, %.f} canvasLocation: {%.f, %.f}", location.x, location.y, canvasLocation.x, canvasLocation.y);
}

- (void)mouseUp:(NSEvent *)event {
    if (_editor.expired()) {
        return;
    }
    auto editor = _editor.lock();
    NSPoint location = [self convertPoint:[event locationInWindow] fromView:nil];
    auto canvasLocation = [self toCanvasPoint:tgfx::Point::Make(static_cast<float>(location.x), static_cast<float>(location.y)) editor:editor.get()];

    NSLog(@"mouseUp location: {%.f, %.f} canvasLocation: {%.f, %.f}", location.x, location.y, canvasLocation.x, canvasLocation.y);
}

@end
