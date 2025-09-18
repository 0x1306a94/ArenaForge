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
//  RendererState.h
//  arenaforge
//
//  Created by king on 2025/9/8.
//

#ifndef RendererState_h_ArenaForge
#define RendererState_h_ArenaForge

#include <arenaforge_core/Point.h>
#include <arenaforge_core/Size.h>

namespace arenaforge::editor {
class RendererState {
  public:
    explicit RendererState(const arenaforge::Size &boundsSize = {1280, 720}, float density = 1.0f);
    ~RendererState();
    /// canvas 尺寸
    const arenaforge::Size &getBoundsSize() const;

    float density() const;

    /// 当前缩放比例
    float zoomScale() const;

    /// 当前滑动偏移
    const arenaforge::Point &contentOffset() const;

    bool updateBounds(const arenaforge::Size &boundsSize, float density);
    bool updateZoomAndOffset(float zoomScale, const arenaforge::Point &contentOffset);

  private:
    /// canvas 尺寸
    arenaforge::Size _boundsSize{1280, 720};
    float _density{1.0f};
    float _zoomScale = 1.0f;
    arenaforge::Point _contentOffset = {};
};
};  // namespace arenaforge::editor

#endif /* RendererState_h_ArenaForge */
