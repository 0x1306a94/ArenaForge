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
//  ShapeCreatorContextImpl.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/29.
//

import arenaforge_editor
import Foundation

final class ShapeCreatorContextImpl: ShapeCreatorContext {
    let editor: AFEditor
    var onShapeAdd: ((AFShapeLayer) -> Void)?
    var onShapeRemove: ((AFShapeLayer) -> Void)?
    init(editor: AFEditor) {
        self.editor = editor
    }
    
    func toCanvasPoint(source: CGPoint) -> CGPoint {
        let inPixel = editor.toCanvasPoint(source)
        return inPixel
    }
    
    func toContainerShapePoint(source: CGPoint) -> CGPoint {
        let rootLayer = editor.project.root
        let local = editor.global(toLocal: source, targetLayer: rootLayer)
        return local
    }
    
    func createShape(type: AFBuiltinShapeType) -> AFShapeLayer? {
        if type == .line {
            let line = editor.project.createLineLayer()
            return line
        }
        
        let shape = editor.project.createShapeLayer(type)
        return shape
    }
    
    func addShape(shape: AFShapeLayer) {
        let rootLayer = editor.project.root
        editor.add(shape, toParent: rootLayer)
        onShapeAdd?(shape)
    }
    
    func removeShape(shape: AFShapeLayer) {
        shape.removeFromParent()
        onShapeRemove?(shape)
    }
}
