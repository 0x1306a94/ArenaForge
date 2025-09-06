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

@interface AFMacCanvasView ()
@property (nonatomic, assign) CVDisplayLinkRef cvDisplayLink;
@property (nonatomic, strong) CADisplayLink *caDisplayLink;
@end

static CVReturn OnDisplayLinkCallback(CVDisplayLinkRef, const CVTimeStamp *, const CVTimeStamp *, CVOptionFlags, CVOptionFlags *, void *userInfo) {
    AFMacCanvasView *view = (__bridge AFMacCanvasView *)userInfo;
    [view draw];
    return kCVReturnSuccess;
}

@implementation AFMacCanvasView {
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

- (void)updateSize {
}

- (void)viewDidMoveToWindow {
    [super viewDidMoveToWindow];

    [self.window makeFirstResponder:self];

    if (@available(macOS 14, *)) {
        self.caDisplayLink = [self displayLinkWithTarget:self selector:@selector(draw)];
    } else {
        CVDisplayLinkCreateWithActiveCGDisplays(&_cvDisplayLink);
        CVDisplayLinkSetOutputCallback(_cvDisplayLink, &OnDisplayLinkCallback, (__bridge void *)self);
    }

    [self updateSize];
}

- (void)startDisplayLink {
    if (@available(macOS 14, *)) {
        if (self.caDisplayLink) {
            [self.caDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        }
    } else {
        if (self.cvDisplayLink) {
            CVDisplayLinkStart(self.cvDisplayLink);
        }
    }
}

- (void)stopDisplayLink {
    if (@available(macOS 14, *)) {
        if (self.caDisplayLink) {
            [self.caDisplayLink removeFromRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
        }
    } else {
        if (self.cvDisplayLink) {
            CVDisplayLinkStop(self.cvDisplayLink);
        }
    }
}

- (void)dealloc {
    if (@available(macOS 14, *)) {
        if (self.caDisplayLink) {
            [self.caDisplayLink invalidate];
            self.caDisplayLink = nil;
        }
    } else {
        if (self.cvDisplayLink) {
            CVDisplayLinkStop(self.cvDisplayLink);
            CVDisplayLinkRelease(self.cvDisplayLink);
        }
    }
}

- (void)draw {
}

@end
