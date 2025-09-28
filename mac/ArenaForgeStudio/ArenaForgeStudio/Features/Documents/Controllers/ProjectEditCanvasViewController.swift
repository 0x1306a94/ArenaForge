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
//  ProjectEditCanvasViewController.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/7.
//

import AppKit
import arenaforge_editor
import Combine

protocol ProjectEditCanvasViewControllerDelegate: AnyObject {
    func projectEditCanvasViewController(_ controller: ProjectEditCanvasViewController, didNewShape shape: AFLayer)
    func projectEditCanvasViewController(_ controller: ProjectEditCanvasViewController, didSelected shape: AFLayer?)
}

final class ProjectEditCanvasViewController: NSViewController {
    private weak var project: ProjectDocument?
    private weak var editor: EditorViewModel?
    private var canvasView: AFMacCanvasView!

    private var trackingArea: NSTrackingArea?

    private var createMouseStartPoint: NSPoint?

    private weak var createShape: AFShapeLayer?

    weak var delegate: ProjectEditCanvasViewControllerDelegate?

    private var minimumZoomScale: CGFloat = 0.1
    private var maximumZoomScale: CGFloat = 30.0
    private var mouseScaleRatio: CGFloat = 120.0
    private var mouseScrollRatio: CGFloat = 0.8
    private var mousePosition: NSPoint = .zero

    private var needAutomaticallyAdjustZoomLevel = true

    var cancellables = Set<AnyCancellable>()

    init(project: ProjectDocument, editor: EditorViewModel) {
        super.init(nibName: nil, bundle: nil)
        self.project = project
        self.editor = editor
    }

    @available(*, unavailable)
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    override func viewDidLoad() {
        super.viewDidLoad()

        setupCanvasView()

        setupEditor()
    }

    override func viewDidAppear() {
        super.viewDidAppear()
        if needAutomaticallyAdjustZoomLevel {
            automaticallyAdjustZoomLevel()
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

    private func setupEditor() {
        guard let project, let editor = editor?.editor, let canvasView else {
            return
        }
        editor.setupCanvasView(canvasView)

        project.$activateEditorToolbarItem.receive(on: RunLoop.main)
            .removeDuplicates()
            .sink { [weak self] in
                if $0 != .cursors {
                    self?.editor?.clearHoverWireframe()
                }
            }
            .store(in: &cancellables)
    }

    private func updateTrackingArea() {
        if let trackingArea {
            view.removeTrackingArea(trackingArea)
            self.trackingArea = nil
        }

        guard !canvasView.bounds.isEmpty else {
            return
        }

        let options: NSTrackingArea.Options = [
            .mouseEnteredAndExited,
            .mouseMoved,
            .activeAlways,
        ]

        var frame = canvasView.frame
        frame = view.convert(frame, from: canvasView)

        let trackingArea = NSTrackingArea(rect: frame, options: options, owner: self)
        self.trackingArea = trackingArea
        view.addTrackingArea(trackingArea)
    }

    private func automaticallyAdjustZoomLevel() {
        needAutomaticallyAdjustZoomLevel = false
        editor?.editor.autoAdjustCanvasScaleForContent()
    }

    override func mouseDown(with event: NSEvent) {
        guard let project, let editor = editor?.editor else {
            project?.activateEditorToolbarItem = .cursors
            return
        }

        guard let rootLayer = project.project?.root else {
            project.activateEditorToolbarItem = .cursors
            return
        }

        let location = canvasView.convert(event.locationInWindow, from: nil)
        let canvasLocation = toCanvasPoint(source: location)

        if project.activateEditorToolbarItem == .cursors, event.clickCount == 2 {
            let pickLayer = editor.findLayer(at: canvasLocation)
            delegate?.projectEditCanvasViewController(self, didSelected: pickLayer)
            return
        }

        createMouseStartPoint = canvasLocation

        switch project.activateEditorToolbarItem {
        case .cursors:
            break
        case .shape(let type):
            guard let shape = createShape(editor: editor, type: type) else {
                return
            }

            guard editor.add(shape, toParent: rootLayer) else {
                return
            }

            let color = NSColor(calibratedRed: CGFloat.random(in: 0.0...1.0), green: CGFloat.random(in: 0.0...1.0), blue: CGFloat.random(in: 0.0...1.0), alpha: 1.0)
            if let line = shape as? AFLineLayer {
                line.strokeColor = color
                line.lineWidth = 4
            } else {
                shape.fillColor = color
            }

            createShape = shape
        }
    }

    override func mouseEntered(with event: NSEvent) {
        updateHoverWireframe(with: event)
    }

    override func mouseMoved(with event: NSEvent) {
        updateHoverWireframe(with: event)
    }

    override func mouseExited(with event: NSEvent) {
        self.editor?.clearHoverWireframe()
    }

    override func mouseDragged(with event: NSEvent) {
        guard let project, project.activateEditorToolbarItem != .cursors, let editor = editor?.editor else {
            project?.activateEditorToolbarItem = .cursors
            return
        }

        guard let createMouseStartPoint else {
            project.activateEditorToolbarItem = .cursors
            return
        }

        guard let rootLayer = project.project?.root else {
            project.activateEditorToolbarItem = .cursors
            return
        }

        let location = canvasView.convert(event.locationInWindow, from: nil)
        let canvasLocation = toCanvasPoint(source: location)

        switch project.activateEditorToolbarItem {
        case .cursors:
            break
        case .shape:
            guard let createShape else {
                return
            }

            let startPoint = editor.global(toLocal: createMouseStartPoint, targetLayer: rootLayer)
            let endPoint = editor.global(toLocal: canvasLocation, targetLayer: rootLayer)

            if let lineLayer = createShape as? AFLineLayer {
                lineLayer.updateStart(startPoint, end: endPoint)
            } else {
                let rect = computeRect(start: startPoint, end: endPoint)
                createShape.frame = rect
            }
        }
    }

    override func mouseUp(with event: NSEvent) {
        guard let project, project.activateEditorToolbarItem != .cursors, let editor = editor?.editor else {
            project?.activateEditorToolbarItem = .cursors
            return
        }

        guard let createMouseStartPoint else {
            project.activateEditorToolbarItem = .cursors
            return
        }

        guard let rootLayer = project.project?.root else {
            project.activateEditorToolbarItem = .cursors
            return
        }

        let location = canvasView.convert(event.locationInWindow, from: nil)
        var canvasLocation = toCanvasPoint(source: location)

        switch project.activateEditorToolbarItem {
        case .cursors:
            break
        case .shape:
            guard let createShape else {
                return
            }

            if createMouseStartPoint == canvasLocation {
                canvasLocation.x = createMouseStartPoint.x + 100
                canvasLocation.y = createMouseStartPoint.y + 100
            }

            let startPoint = editor.global(toLocal: createMouseStartPoint, targetLayer: rootLayer)
            let endPoint = editor.global(toLocal: canvasLocation, targetLayer: rootLayer)
            if let lineLayer = createShape as? AFLineLayer {
                lineLayer.updateStart(startPoint, end: endPoint)
            } else {
                let rect = computeRect(start: startPoint, end: endPoint)
                createShape.frame = rect
            }

            project.activateEditorToolbarItem = .cursors
            delegate?.projectEditCanvasViewController(self, didNewShape: createShape)
            self.createShape = nil
        }
    }

    override func scrollWheel(with event: NSEvent) {
        guard let editor = editor?.editor else {
            return
        }

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

    override func magnify(with event: NSEvent) {
        guard let editor = editor?.editor else {
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

    private func createShape(editor: AFEditor, type: AFBuiltinShapeType) -> AFShapeLayer? {
        switch type {
        case .rectangle:
            return editor.project.createShapeLayer(.rectangle)
        case .ellipse:
            return editor.project.createShapeLayer(.ellipse)
        case .line:
            return editor.project.createLineLayer()
        default:
            return nil
        }
    }

    private func updateHoverWireframe(with event: NSEvent) {
        guard let editor, let project, project.activateEditorToolbarItem == .cursors else {
            return
        }

        let location = canvasView.convert(event.locationInWindow, from: nil)
        let canvasLocation = toCanvasPoint(source: location)

        guard let targetLayer = editor.findLayer(at: canvasLocation) else {
            editor.clearHoverWireframe()
            return
        }

        editor.createHoverWireframeLayer(targetLayer: targetLayer)
    }

    private func toCanvasPoint(source: CGPoint) -> CGPoint {
        guard let editor = editor?.editor else { return source }

        let currentZoom = editor.zoomScale()
        let density = editor.density()
        let contentOffset = editor.contentOffset()

        let px = source.x * density
        let py = source.y * density
        let x = (px - contentOffset.x) / currentZoom
        let y = (py - contentOffset.y) / currentZoom

        return CGPoint(x: x, y: y)
    }

    private func updateZooming(scaleFactor: CGFloat) {
        guard let editor = editor?.editor else {
            return
        }

        let currentZoom = editor.zoomScale()
        var contentOffset = editor.contentOffset()

        let newZoom = max(minimumZoomScale, min(maximumZoomScale, currentZoom * scaleFactor))
        contentOffset.x = (contentOffset.x - mousePosition.x) * (newZoom / currentZoom) + mousePosition.x
        contentOffset.y = (contentOffset.y - mousePosition.y) * (newZoom / currentZoom) + mousePosition.y

        editor.updateZoomScale(newZoom, offset: contentOffset)
    }

    private func computeRect(start: CGPoint, end: CGPoint) -> CGRect {
        let x = min(start.x, end.x)
        let y = min(start.y, end.y)
        let w = abs(end.x - start.x)
        let h = abs(end.y - start.y)
        return CGRect(x: x, y: y, width: w, height: h)
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
