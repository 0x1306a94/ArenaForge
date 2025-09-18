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
//  Point.h
//  ArenaForge
//
//  Created by king on 2025/9/17.
//

#ifndef Point_h_ArenaForge
#define Point_h_ArenaForge

#include <cmath>

#include <arenaforge_core/Size.h>

namespace arenaforge {
/**
 * Point holds two 32-bit floating point coordinates.
 */
struct Point {
    /**
     * Creates a Point set to (0, 0).
     */
    static const Point &Zero() {
        static const Point zero = Point::Make(0, 0);
        return zero;
    }

    /**
     * Creates a Point with specified x and y value.
     */
    static constexpr Point Make(float x, float y) {
        return {x, y};
    }

    /**
     * Creates a Point with specified x and y value.
     */
    static constexpr Point Make(int x, int y) {
        return {static_cast<float>(x), static_cast<float>(y)};
    }

    /**
     * Constructs a Point set to (0, 0).
     */
    constexpr Point()
        : x(0)
        , y(0) {
    }

    /**
     * Constructs a Point set to (x, y).
     */
    constexpr Point(float x, float y)
        : x(x)
        , y(y) {
    }

    /**
     * x-axis value.
     */
    float x;
    /**
     * y-axis value.
     */
    float y;

    /**
     * Returns true if x and y are both zero.
     */
    bool isZero() const {
        return (0 == x) && (0 == y);
    }

    /**
     * Sets x to xValue and y to yValue.
     */
    void set(float xValue, float yValue) {
        x = xValue;
        y = yValue;
    }

    /**
     * Adds offset (dx, dy) to Point.
     */
    void offset(float dx, float dy) {
        x += dx;
        y += dy;
    }

    /**
     * Returns the Euclidean distance from origin.
     */
    float length() const {
        return Point::Length(x, y);
    }

    /**
     * Returns true if a is equivalent to b.
     */
    friend bool operator==(const Point &a, const Point &b) {
        return a.x == b.x && a.y == b.y;
    }

    /**
     * Returns true if a is not equivalent to b.
     */
    friend bool operator!=(const Point &a, const Point &b) {
        return a.x != b.x || a.y != b.y;
    }

    /**
     * Returns a Point from b to a; computed as (a.x - b.x, a.y - b.y).
     */
    friend Point operator-(const Point &a, const Point &b) {
        return {a.x - b.x, a.y - b.y};
    }

    /**
     * Subtracts vector Point v from Point. Sets Point to: (x - v.x, y - v.y).
     */
    void operator-=(const Point &v) {
        x -= v.x;
        y -= v.y;
    }

    /**
     * Returns Point resulting from Point a offset by Point b, computed as:
     * (a.x + b.x, a.y + b.y).
     */
    friend Point operator+(const Point &a, const Point &b) {
        return {a.x + b.x, a.y + b.y};
    }

    /**
     * offset vector point v from Point. Sets Point to: (x + v.x, y + v.y).
     */
    void operator+=(const Point &v) {
        x += v.x;
        y += v.y;
    }

    /**
     * Returns Point multiplied by scale.
     * (x * scale, y * scale)
     */
    friend Point operator*(const Point &p, float scale) {
        return {p.x * scale, p.y * scale};
    }

    /**
     * Multiplies Point by scale. Sets Point to: (x * scale, y * scale).
     */
    void operator*=(float scale) {
        x *= scale;
        y *= scale;
    }

    /**
     * Returns Point multiplied by size.
     * (x * size.width, y * size.height)
     */
    friend Point operator*(const Point &p, const Size &size) {
        return {p.x * size.width, p.y * size.height};
    }

    /**
     * Returns the Euclidean distance from origin.
     */
    static float Length(float x, float y) {
        return sqrt(x * x + y * y);
    }

    /**
     * Returns the Euclidean distance between a and b.
     */
    static float Distance(const Point &a, const Point &b) {
        return Length(a.x - b.x, a.y - b.y);
    }
};
};  // namespace arenaforge

#endif /* Point_h_ArenaForge */
