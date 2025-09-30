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

enum CanvasInteractionMode {
    case cursor
    case shape(ShapeCreator)
}

final class ProjectEditCanvasViewController: NSViewController {
    private weak var project: ProjectDocument?
    private weak var editor: EditorViewModel?
    private var canvasView: AFMacCanvasView!
    private var hoverManager: LayerHoverManager?
    private var zoomManager: ZoomManager?
    private var interactionMode: CanvasInteractionMode = .cursor

    private var trackingArea: NSTrackingArea?

    weak var delegate: ProjectEditCanvasViewControllerDelegate?
    weak var selectionManager: SelectionManager?

    private var needAutomaticallyAdjustZoomLevel = true

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
        self.hoverManager = LayerHoverManager(editor: editor.editor)
        self.zoomManager = ZoomManager(editor: editor.editor)
    }

    @available(*, unavailable)
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    override func viewDidLoad() {
        super.viewDidLoad()

        setupCanvasView()

        setupTrackingArea()

        setupEditor()
    }

    override func viewDidAppear() {
        super.viewDidAppear()
        if needAutomaticallyAdjustZoomLevel {
            automaticallyAdjustZoomLevel()
        }
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
                    self?.hoverManager?.clearHoverWireframe()
                }
            }
            .store(in: &cancellables)
    }

    private func setupTrackingArea() {
        if let trackingArea {
            view.removeTrackingArea(trackingArea)
            self.trackingArea = nil
        }

        let options: NSTrackingArea.Options = [
            .mouseEnteredAndExited,
            .mouseMoved,
            .activeAlways,
            .inVisibleRect,
        ]

        let trackingArea = NSTrackingArea(rect: .zero, options: options, owner: self)
        self.trackingArea = trackingArea
        view.addTrackingArea(trackingArea)
    }

    private func automaticallyAdjustZoomLevel() {
        needAutomaticallyAdjustZoomLevel = false
        zoomManager?.automaticallyAdjustZoomLevel()
    }

    private func handleCursorMouseDown(with event: NSEvent) {
        guard let project, let editor = editor?.editor else { return }

        let location = canvasView.convert(event.locationInWindow, from: nil)
        let canvasLocation = editor.toCanvasPoint(location)

        if !editor.hitTestPoint(inSelectedBoundingBox: canvasLocation) {
            let pickLayer = editor.findLayer(at: canvasLocation) ?? project.project?.root
            delegate?.projectEditCanvasViewController(self, didSelected: pickLayer)
        }
    }

    override func mouseDown(with event: NSEvent) {
        guard let project, let editor = editor?.editor else { return }

        switch project.activateEditorToolbarItem {
        case .cursors:
            interactionMode = .cursor
            handleCursorMouseDown(with: event)
            hoverManager?.clearHoverWireframe()
        case .shape(let type):
            if let creator = makeShapeCreator(editor: editor, type: type) {
                interactionMode = .shape(creator)
                creator.mouseDown(with: event)
            }
        }
    }

    override func mouseDragged(with event: NSEvent) {
        switch interactionMode {
        case .cursor:
            let location = canvasView.convert(event.locationInWindow, from: nil)
            self.selectionManager?.updateMove(loaction: location)
        case .shape(let creator):
            creator.mouseDragged(with: event)
        }
    }

    override func mouseUp(with event: NSEvent) {
        defer {
            interactionMode = .cursor
            project?.activateEditorToolbarItem = .cursors
        }

        switch interactionMode {
        case .cursor:
            self.selectionManager?.endMove()
        case .shape(let creator):
            creator.mouseUp(with: event)
        }
    }

    override func mouseEntered(with event: NSEvent) {
        updateHoverWireframe(with: event)
    }

    override func mouseMoved(with event: NSEvent) {
        updateHoverWireframe(with: event)
    }

    override func mouseExited(with event: NSEvent) {
        self.hoverManager?.clearHoverWireframe()
    }

    override func scrollWheel(with event: NSEvent) {
        zoomManager?.scrollWheel(with: event, canvasView: canvasView)
    }

    override func magnify(with event: NSEvent) {
        zoomManager?.magnify(with: event, canvasView: canvasView)
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
        guard let project, project.activateEditorToolbarItem == .cursors else {
            return
        }

        let location = canvasView.convert(event.locationInWindow, from: nil)
        self.hoverManager?.updateHoverWireframe(with: location)
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
