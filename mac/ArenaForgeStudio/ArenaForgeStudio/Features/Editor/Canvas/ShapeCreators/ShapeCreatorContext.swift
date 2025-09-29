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
//  ShapeCreatorContext.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/29.
//

import arenaforge_editor
import Foundation

protocol ShapeCreatorContext {
    func toCanvasPoint(source: CGPoint) -> CGPoint
    func toContainerShapePoint(source: CGPoint) -> CGPoint
    func createShape(type: AFBuiltinShapeType) -> AFShapeLayer?
    func addShape(shape: AFShapeLayer)
    func removeShape(shape: AFShapeLayer)
    func defaultFillColor() -> NSColor
    func defaultStrokeColor() -> NSColor
    func defaultLineWidth() -> CGFloat
}

extension ShapeCreatorContext {
    func defaultFillColor() -> NSColor {
        NSColor(red: CGFloat.random(in: 0.0...1.0), green: CGFloat.random(in: 0.0...1.0), blue: CGFloat.random(in: 0.0...1.0), alpha: 1.0)
    }

    func defaultStrokeColor() -> NSColor {
        NSColor(red: CGFloat.random(in: 0.0...1.0), green: CGFloat.random(in: 0.0...1.0), blue: CGFloat.random(in: 0.0...1.0), alpha: 1.0)
    }

    func defaultLineWidth() -> CGFloat {
        1.0
    }
}
