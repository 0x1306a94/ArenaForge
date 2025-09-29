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
//  ZoomManager.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/29.
//

import AppKit
import arenaforge_editor
import Foundation

final class ZoomManager {
    weak var editor: AFEditor?
    
    private var minimumZoomScale: CGFloat = 0.1
    private var maximumZoomScale: CGFloat = 30.0
    private var mouseScaleRatio: CGFloat = 120.0
    private var mouseScrollRatio: CGFloat = 0.8
    private var mousePosition: NSPoint = .zero
    
    init(editor: AFEditor) {
        self.editor = editor
    }
    
    func scrollWheel(with event: NSEvent, canvasView: NSView) {
        guard let editor else { return }
        
        let density = editor.density()
        var contentOffset = editor.contentOffset()
        
        let scrollingDeltaX = event.scrollingDeltaX
        let scrollingDeltaY = event.scrollingDeltaY
        
        let modifiers = event.modifierFlags
        
        if modifiers.contains(.control) || modifiers.contains(.command) {
            var location = canvasView.convert(event.locationInWindow, from: nil)
            location.x *= density
            location.y *= density
            mousePosition = location
            
            let scaleFactor = if event.hasPreciseScrollingDeltas {
                1.0 + scrollingDeltaY / mouseScaleRatio
            } else {
                pow(1.1, event.scrollingDeltaY)
            }
            
            updateZooming(scaleFactor: scaleFactor)
        } else {
            var deltaX = scrollingDeltaX // * density
            var deltaY = scrollingDeltaY // * density
            if !event.hasPreciseScrollingDeltas {
                deltaX *= mouseScrollRatio
                deltaY *= mouseScrollRatio
            }
            contentOffset.x += deltaX
            contentOffset.y += deltaY
            editor.updateOffset(contentOffset)
        }
    }
    
    func magnify(with event: NSEvent, canvasView: NSView) {
        guard let editor else { return }
        
        let density = editor.density()
        
        let scaleFactor = 1.0 + event.magnification
        var location = canvasView.convert(event.locationInWindow, from: nil)
        location.x *= density
        location.y *= density
        mousePosition = location
        updateZooming(scaleFactor: scaleFactor)
    }
    
    func automaticallyAdjustZoomLevel() {
        editor?.autoAdjustCanvasScaleForContent()
    }
    
    #if DEBUG
        deinit {
            Swift.print("\(type(of: self)) deinit")
        }
    #endif
}

extension ZoomManager {
    private func updateZooming(scaleFactor: CGFloat) {
        guard let editor else { return }

        let currentZoom = editor.zoomScale()
        var contentOffset = editor.contentOffset()

        let newZoom = max(minimumZoomScale, min(maximumZoomScale, currentZoom * scaleFactor))
        contentOffset.x = (contentOffset.x - mousePosition.x) * (newZoom / currentZoom) + mousePosition.x
        contentOffset.y = (contentOffset.y - mousePosition.y) * (newZoom / currentZoom) + mousePosition.y

        editor.updateZoomScale(newZoom, offset: contentOffset)
    }
}
