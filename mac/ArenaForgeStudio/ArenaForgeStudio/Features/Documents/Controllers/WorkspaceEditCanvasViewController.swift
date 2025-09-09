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
//  WorkspaceEditCanvasViewController.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/7.
//

import AppKit
import arenaforge_editor

final class WorkspaceEditCanvasViewController: NSViewController {
    weak var workspace: WorkspaceDocument?
    weak var editor: AFEditor?
    var canvasView: AFMacCanvasView!

    var trackingArea: NSTrackingArea?

    var createVenueStartPoint: NSPoint?
    var createVenue: AFVenue?

    private var minimumZoomScale: CGFloat = 0.1
    private var maximumZoomScale: CGFloat = 30.0
    private var mouseScaleRatio: CGFloat = 300.0
    private var mouseScrollRatio: CGFloat = 0.8
    private var mousePosition: NSPoint = .zero

    init(workspace: WorkspaceDocument, editor: AFEditor) {
        super.init(nibName: nil, bundle: nil)
        self.workspace = workspace
        self.editor = editor
    }

    @available(*, unavailable)
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    override func viewDidLoad() {
        super.viewDidLoad()

        setupCanvasView()

        if let editor, let canvasView {
            editor.setupCanvasView(canvasView)
        }
    }

    override func viewDidLayout() {
        super.viewDidLayout()

        updateTrackingArea()
    }

    private func setupCanvasView() {
        let canvasView = AFMacCanvasView()
        canvasView.translatesAutoresizingMaskIntoConstraints = false

        view.addSubview(canvasView)
        self.canvasView = canvasView
        NSLayoutConstraint.activate([
            canvasView.leadingAnchor.constraint(equalTo: view.leadingAnchor),
            canvasView.topAnchor.constraint(equalTo: view.safeAreaLayoutGuide.topAnchor),
            canvasView.trailingAnchor.constraint(equalTo: view.trailingAnchor),
            canvasView.bottomAnchor.constraint(equalTo: view.bottomAnchor),
        ])
    }

    private func updateTrackingArea() {
        if let trackingArea {
            view.removeTrackingArea(trackingArea)
            self.trackingArea = nil
        }

        guard !self.canvasView.bounds.isEmpty else {
            return
        }

        let options: NSTrackingArea.Options = [
            .mouseEnteredAndExited,
            .mouseMoved,
            .activeAlways,
        ]

        var frame = self.canvasView.frame
        frame = self.view.convert(frame, from: self.canvasView)

        let trackingArea = NSTrackingArea(rect: frame, options: options, owner: self)
        self.trackingArea = trackingArea
        view.addTrackingArea(trackingArea)
    }

    override func mouseDown(with event: NSEvent) {
        guard let workspace, let editor, workspace.activateEditorToolbarItem == .venue else {
            return
        }

        let veune = editor.project.createVenue()

        let location = canvasView.convert(event.locationInWindow, from: nil)
        let canvasLocation = toCanvasPoint(source: location)
        createVenueStartPoint = canvasLocation
        createVenue = veune
    }

    override func mouseMoved(with event: NSEvent) {
//        let location = self.canvasView.convert(event.locationInWindow, from: nil)
//        let canvasLocation = toCanvasPoint(source: location)
    }

    override func mouseDragged(with event: NSEvent) {
        guard let createVenueStartPoint, let createVenue else {
            return
        }
        let location = canvasView.convert(event.locationInWindow, from: nil)
        let canvasLocation = toCanvasPoint(source: location)

        let rect = computeRect(start: createVenueStartPoint, end: canvasLocation)
        createVenue.frame = rect
    }

    override func mouseUp(with event: NSEvent) {
        guard let createVenueStartPoint, let createVenue else {
            return
        }
        let location = canvasView.convert(event.locationInWindow, from: nil)
        var canvasLocation = toCanvasPoint(source: location)

        if createVenueStartPoint == canvasLocation {
            canvasLocation.x = createVenueStartPoint.x + 300
            canvasLocation.y = createVenueStartPoint.y + 300
        }

        let rect = computeRect(start: createVenueStartPoint, end: canvasLocation)
        createVenue.frame = rect

        self.createVenue = nil

        workspace?.activateEditorToolbarItem = .cursors
    }

    override func scrollWheel(with event: NSEvent) {
        guard let editor else {
            return
        }

        let density = editor.density()
        var contentOffset = editor.contentOffset()

        let scrollingDeltaX = event.scrollingDeltaX
        let scrollingDeltaY = event.scrollingDeltaY

        let modifiers = event.modifierFlags
        let isShiftPressed = modifiers.contains(.shift)
        let isControlPressed = modifiers.contains(.control)
        let isCommandPressed = modifiers.contains(.command)

        if isControlPressed || isCommandPressed {
            var location = canvasView.convert(event.locationInWindow, from: nil)
            location.x *= density
            location.y *= density
            mousePosition = location

            let scaleFactor = exp(scrollingDeltaY / mouseScaleRatio)
            updateZooming(scaleFactor: scaleFactor)
        } else {
            var deltaX = scrollingDeltaX * density * mouseScrollRatio
            var deltaY = scrollingDeltaY * density * mouseScrollRatio
            if isShiftPressed, deltaX == 0.0, deltaY != 0.0 {
                deltaX = deltaY
                deltaY = 0
            }
            contentOffset.x += deltaX
            contentOffset.y += deltaY
            editor.updateOffset(contentOffset)
        }
    }

    override func magnify(with event: NSEvent) {
        guard let editor else {
            super.magnify(with: event)
            return
        }

        let density = editor.density()

        let scaleFactor = 1.0 + event.magnification
        var location = canvasView.convert(event.locationInWindow, from: nil)
        location.x *= density
        location.y *= density
        mousePosition = location
        updateZooming(scaleFactor: scaleFactor)
    }

    private func toCanvasPoint(source: NSPoint) -> NSPoint {
        guard let editor else { return source }

        let currentZoom = editor.zoomScale()
        let density = editor.density()
        let contentOffset = editor.contentOffset()

        let px = source.x * density
        let py = source.y * density
        let x = (px - contentOffset.x) / currentZoom
        let y = (py - contentOffset.y) / currentZoom

        return NSPoint(x: x, y: y)
    }

    private func updateZooming(scaleFactor: CGFloat) {
        guard let editor else {
            return
        }

        let currentZoom = editor.zoomScale()
        var contentOffset = editor.contentOffset()

        let newZoom = max(minimumZoomScale, min(maximumZoomScale, currentZoom * scaleFactor))
        contentOffset.x = (contentOffset.x - mousePosition.x) * (newZoom / currentZoom) + mousePosition.x
        contentOffset.y = (contentOffset.y - mousePosition.y) * (newZoom / currentZoom) + mousePosition.y

        editor.updateZoomScale(newZoom, offset: contentOffset)
    }

    private func computeRect(start: NSPoint, end: NSPoint) -> NSRect {
        let x = min(start.x, end.x)
        let y = min(start.y, end.y)
        let w = abs(end.x - start.x)
        let h = abs(end.y - start.y)
        return NSRect(x: x, y: y, width: w, height: h)
    }

    deinit {
        if let trackingArea {
            self.view.removeTrackingArea(trackingArea)
            self.trackingArea = nil
        }
#if DEBUG
        print("\(type(of: self)) deinit")
#endif
    }
}
