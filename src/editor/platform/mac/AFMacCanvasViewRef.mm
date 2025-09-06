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
//  AFMacCanvasViewRef.mm
//
//
//  Created by KK on 2025/9/6.
//

#import "AFMacCanvasViewRef.h"

#include <memory>

#include <CoreFoundation/CFBase.h>

#import "AFMacCanvasView+Private.h"
#import "MacRendererBackend.h"

namespace arenaforge::editor {
AFMacCanvasViewRef::AFMacCanvasViewRef(void *hostView)
    : _hostView(hostView)
    , _rendererBackend(nullptr) {
    if (_hostView) {
        CFRetain(_hostView);
        AFMacCanvasView *hostView = (__bridge AFMacCanvasView *)_hostView;
        _rendererBackend = std::make_shared<MacRendererBackend>(hostView);
    }
}

AFMacCanvasViewRef::~AFMacCanvasViewRef() {
    if (_hostView) {
        CFRelease(_hostView);
        _hostView = nullptr;
    }
    _rendererBackend = nullptr;
}

AFMacCanvasViewRef::AFMacCanvasViewRef(AFMacCanvasViewRef &&other) noexcept
    : _hostView(other._hostView)
    , _rendererBackend(nullptr) {
    other._hostView = nullptr;
    if (_hostView != nullptr) {
        AFMacCanvasView *hostView = (__bridge AFMacCanvasView *)_hostView;
        _rendererBackend = std::make_shared<MacRendererBackend>(hostView);
    }
}

AFMacCanvasViewRef &AFMacCanvasViewRef::operator=(AFMacCanvasViewRef &&other) noexcept {
    if (this != &other) {
        _hostView = other._hostView;

        if (_hostView != nullptr) {
            AFMacCanvasView *hostView = (__bridge AFMacCanvasView *)_hostView;
            _rendererBackend = std::make_shared<MacRendererBackend>(hostView);
        } else {
            _rendererBackend = nullptr;
        }

        other._hostView = nullptr;
        other._rendererBackend = nullptr;
    }
    return *this;
}

void AFMacCanvasViewRef::startDisplayLink() {
    if (_hostView == nullptr) {
        return;
    }

    AFMacCanvasView *hostView = (__bridge AFMacCanvasView *)_hostView;
    [hostView startDisplayLink];
}

void AFMacCanvasViewRef::stopDisplayLink() {
    if (_hostView == nullptr) {
        return;
    }

    AFMacCanvasView *hostView = (__bridge AFMacCanvasView *)_hostView;
    [hostView stopDisplayLink];
}

std::shared_ptr<RendererBackend> AFMacCanvasViewRef::getRendererBackend() {
    return _rendererBackend;
}

};  // namespace arenaforge::editor
