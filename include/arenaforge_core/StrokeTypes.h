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
//  StrokeTypes.h
//  ArenaForge
//
//  Created by king on 2025/9/17.
//

#ifndef StrokeAlign_h_ArenaForge
#define StrokeAlign_h_ArenaForge

namespace arenaforge {
/**
 * The alignment of the stroke relative to the boundaries of the shape.
 */
enum class StrokeAlign {
    /**
     * Draw a stroke centered along the shape boundary.
     */
    Center,
    /**
     * Draw a stroke inside the shape boundary.
     */
    Inside,
    /**
     * Draw a stroke outside the shape boundary.
     */
    Outside
};

/**
 * LineCap draws at the beginning and end of an open path contour.
 */
enum class LineCap {
    /**
     * No stroke extension.
     */
    Butt,
    /**
     * Adds circle
     */
    Round,
    /**
     * Adds square
     */
    Square
};

/**
 * LineJoin specifies how corners are drawn when a shape is stroked. Join affects the four corners
 * of a stroked rectangle, and the connected segments in a stroked path. Choose miter join to draw
 * sharp corners. Choose round join to draw a circle with a radius equal to the stroke width on
 * top of the corner. Choose bevel join to minimally connect the thick strokes.
 */
enum class LineJoin {
    /**
     * Extends to miter limit.
     */
    Miter,
    /**
     * Adds circle.
     */
    Round,
    /**
     * Connects outside edges.
     */
    Bevel
};
};  // namespace arenaforge

#endif /* StrokeAlign_h_ArenaForge */
