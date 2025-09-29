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

    weak var delegate: ProjectEditCanvasViewControllerDelegate?

    private var minimumZoomScale: CGFloat = 0.1
    private var maximumZoomScale: CGFloat = 30.0
    private var mouseScaleRatio: CGFloat = 120.0
    private var mouseScrollRatio: CGFloat = 0.8
    private var mousePosition: NSPoint = .zero

    private var needAutomaticallyAdjustZoomLevel = true

    private var currentShapeCreator: (any ShapeCreator)?

    var cancellables = Set<AnyCancellable>()

    private let shapeCreatorFactories: [AFBuiltinShapeType: (AFMacCanvasView, ShapeCreatorContext) -> ShapeCreator] = [
        .rectangle: { RectangleCreator(canvasView: $0, context: $1) },
        .ellipse: { EllipseCreator(canvasView: $0, context: $1) },
        .triangle: { TriangleCreator(canvasView: $0, context: $1) },
        .line: { LineShapeCreator(canvasView: $0, context: $1) },
    ]

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

        switch project.activateEditorToolbarItem {
        case .cursors:
            let location = canvasView.convert(event.locationInWindow, from: nil)
            let canvasLocation = editor.toCanvasPoint(location)
            let pickLayer = editor.findLayer(at: canvasLocation) ?? project.project?.root
            delegate?.projectEditCanvasViewController(self, didSelected: pickLayer)

        case .shape(let type):
            guard let shapeCreator = makeShapeCreator(editor: editor, type: type) else {
                self.currentShapeCreator = nil
                return
            }

            self.currentShapeCreator = shapeCreator
            shapeCreator.mouseDown(with: event)
        }
    }

    override func mouseDragged(with event: NSEvent) {
        guard let project else {
            project?.activateEditorToolbarItem = .cursors
            return
        }

        switch project.activateEditorToolbarItem {
        case .cursors:
            break
        case .shape:
            currentShapeCreator?.mouseDragged(with: event)
        }
    }

    override func mouseUp(with event: NSEvent) {
        defer { project?.activateEditorToolbarItem = .cursors }
        guard let project else {
            return
        }

        switch project.activateEditorToolbarItem {
        case .cursors:
            break
        case .shape:
            defer {
                project.activateEditorToolbarItem = .cursors
                self.currentShapeCreator = nil
            }
            currentShapeCreator?.mouseUp(with: event)
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

    private func makeShapeCreator(editor: AFEditor, type: AFBuiltinShapeType) -> (any ShapeCreator)? {
        let context = ShapeCreatorContextImpl(editor: editor)
        context.onShapeAdd = { [weak self] in
            guard let self else { return }
            self.delegate?.projectEditCanvasViewController(self, didNewShape: $0)
        }

        return shapeCreatorFactories[type]?(canvasView, context)
    }

    private func updateHoverWireframe(with event: NSEvent) {
        guard let editor, let project, project.activateEditorToolbarItem == .cursors else {
            return
        }

        let location = canvasView.convert(event.locationInWindow, from: nil)
        let canvasLocation = editor.editor.toCanvasPoint(location)

        guard let targetLayer = editor.findLayer(at: canvasLocation) else {
            editor.clearHoverWireframe()
            return
        }

        editor.createHoverWireframeLayer(targetLayer: targetLayer)
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
