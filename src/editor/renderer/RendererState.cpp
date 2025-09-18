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
//  RendererState.cpp
//  arenaforge
//
//  Created by king on 2025/9/8.
//

#include "RendererState.h"

#include <arenaforge_core/Print.h>

namespace arenaforge::editor {
RendererState::RendererState(const arenaforge::Size &boundsSize, float density)
    : _boundsSize(boundsSize)
    , _density(density) {
    PrintLog("%s", __PRETTY_FUNCTION__);
}

RendererState::~RendererState() {
    PrintLog("%s", __PRETTY_FUNCTION__);
}

const arenaforge::Size &RendererState::getBoundsSize() const {
    return _boundsSize;
}

float RendererState::density() const {
    return _density;
}

/// 当前缩放比例
float RendererState::zoomScale() const {
    return _zoomScale;
}

/// 当前滑动偏移
const arenaforge::Point &RendererState::contentOffset() const {
    return _contentOffset;
}

bool RendererState::updateBounds(const arenaforge::Size &boundsSize, float density) {
    if (boundsSize.width <= 0 || boundsSize.height <= 0) {
        PrintError("%s width or height is invalid!", __PRETTY_FUNCTION__);
        return false;
    }

    if (density < 1.0) {
        PrintError("%s density is invalid!", __PRETTY_FUNCTION__);
        return false;
    }

    if (_boundsSize == boundsSize && _density == density) {
        return false;
    }

    _boundsSize = boundsSize;
    _density = density;
    return true;
}

bool RendererState::updateZoomAndOffset(float zoomScale, const arenaforge::Point &contentOffset) {
    if (zoomScale == _zoomScale && contentOffset == _contentOffset) {
        return false;
    }
    _zoomScale = zoomScale;
    _contentOffset = contentOffset;
    return true;
}
};  // namespace arenaforge::editor
