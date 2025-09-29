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
//  RectShapeCreator.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/29.
//

import AppKit
import arenaforge_editor
import Foundation

class RectBasedShapeCreator: ShapeCreator {
    let shapeType: AFBuiltinShapeType
    let canvasView: AFMacCanvasView
    let context: ShapeCreatorContext
    private var startCanvasPoint: CGPoint?
    private var currentShape: AFShapeLayer?

    init(shapeType: AFBuiltinShapeType, canvasView: AFMacCanvasView, context: some ShapeCreatorContext) {
        self.shapeType = shapeType
        self.canvasView = canvasView
        self.context = context
    }

    func mouseDown(with event: NSEvent) {
        let location = canvasView.convert(event.locationInWindow, from: nil)
        startCanvasPoint = context.toCanvasPoint(source: location)
        currentShape = context.createShape(type: shapeType)

        if let currentShape {
            currentShape.fillColor = context.defaultFillColor()
            context.addShape(shape: currentShape)
        }
    }

    func mouseDragged(with event: NSEvent) {
        guard let startCanvasPoint, let currentShape else { return }
        let location = canvasView.convert(event.locationInWindow, from: nil)
        let endCanvasPoint = context.toCanvasPoint(source: location)
        let startPoint = context.toContainerShapePoint(source: startCanvasPoint)
        let currentPoint = context.toContainerShapePoint(source: endCanvasPoint)

        let rect = CGRect(
            x: min(startPoint.x, currentPoint.x),
            y: min(startPoint.y, currentPoint.y),
            width: abs(currentPoint.x - startPoint.x),
            height: abs(currentPoint.y - startPoint.y)
        )
        currentShape.updateFrame(rect)
    }

    func mouseUp(with event: NSEvent) {}
}

final class RectangleCreator: RectBasedShapeCreator {
    convenience init(canvasView: AFMacCanvasView, context: some ShapeCreatorContext) {
        self.init(shapeType: .rectangle, canvasView: canvasView, context: context)
    }
}

final class EllipseCreator: RectBasedShapeCreator {
    convenience init(canvasView: AFMacCanvasView, context: some ShapeCreatorContext) {
        self.init(shapeType: .ellipse, canvasView: canvasView, context: context)
    }
}

final class TriangleCreator: RectBasedShapeCreator {
    convenience init(canvasView: AFMacCanvasView, context: some ShapeCreatorContext) {
        self.init(shapeType: .triangle, canvasView: canvasView, context: context)
    }
}
