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
//  LineShapeCreator.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/29.
//

import AppKit
import arenaforge_editor
import Foundation

final class LineShapeCreator: ShapeCreator {
    let canvasView: AFMacCanvasView
    let context: ShapeCreatorContext
    private var startCanvasPoint: CGPoint?
    private var currentLine: AFLineLayer?

    init(canvasView: AFMacCanvasView, context: some ShapeCreatorContext) {
        self.canvasView = canvasView
        self.context = context
    }

    func mouseDown(with event: NSEvent) {
        let location = canvasView.convert(event.locationInWindow, from: nil)
        startCanvasPoint = context.toCanvasPoint(source: location)
        guard let line = context.createShape(type: .line) as? AFLineLayer else { return }
        line.strokeColor = context.defaultStrokeColor()
        line.lineWidth = context.defaultLineWidth()
        currentLine = line
        context.addShape(shape: line)
    }

    func mouseDragged(with event: NSEvent) {
        guard let startCanvasPoint, let currentLine else { return }
        let location = canvasView.convert(event.locationInWindow, from: nil)
        let endCanvasPoint = context.toCanvasPoint(source: location)
        let startPoint = context.toContainerShapePoint(source: startCanvasPoint)
        let currentPoint = context.toContainerShapePoint(source: endCanvasPoint)
        currentLine.updateStart(startPoint, end: currentPoint)
    }

    func mouseUp(with event: NSEvent) {}
}
