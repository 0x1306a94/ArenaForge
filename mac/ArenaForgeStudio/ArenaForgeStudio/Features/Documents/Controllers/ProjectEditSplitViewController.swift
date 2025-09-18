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
//  ProjectEditSplitViewController.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/7.
//

import AppKit
import arenaforge_editor
import SwiftUI

final class ProjectEditSplitViewController: NSSplitViewController, ProjectEditCanvasViewControllerDelegate {
    static let minSidebarWidth: CGFloat = 242
    static let maxSnapWidth: CGFloat = snapWidth + 10
    static let snapWidth: CGFloat = 272
    static let minSnapWidth: CGFloat = snapWidth - 10

    private weak var project: ProjectDocument?
    private weak var editor: AFEditor?
    private weak var windowRef: NSWindow?
    private unowned var hapticPerformer: NSHapticFeedbackPerformer

    private weak var editCanvas: ProjectEditCanvasViewController?
    private weak var layerNavigator: LayerNavigatorViewController?

    // MARK: - Initialization

    init(
        windowRef: NSWindow,
        project: ProjectDocument,
        editor: AFEditor,
        hapticPerformer: NSHapticFeedbackPerformer = NSHapticFeedbackManager.defaultPerformer
    ) {
        self.windowRef = windowRef
        self.project = project
        self.editor = editor
        self.hapticPerformer = hapticPerformer
        super.init(nibName: nil, bundle: nil)
    }

    @available(*, unavailable)
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    override func viewDidLoad() {
        super.viewDidLoad()
        guard let windowRef else {
            // swiftlint:disable:next line_length
            assertionFailure("No WindowRef found, not initialized properly or the window was dereferenced and the controller was not.")
            return
        }

        guard let project, let editor else {
            // swiftlint:disable:next line_length
            assertionFailure("Missing a workspace model: project=\(project == nil) editor=\(editor == nil)")
            return
        }

        splitView.translatesAutoresizingMaskIntoConstraints = false

        let navigator = makeNavigator(project: project, editor: editor)
        let canvas = makeCanvas(project: project, editor: editor)
        let inspector = makeInspector(view: InspectorAreaView())
        addSplitViewItem(navigator)
        addSplitViewItem(canvas)
        addSplitViewItem(inspector)

        self.editCanvas?.delegate = self
    }

    override func viewWillAppear() {
        super.viewWillAppear()

        splitView.setPosition(Self.minSidebarWidth, ofDividerAt: 0)
    }

    private func makeNavigator(project: ProjectDocument, editor: AFEditor) -> NSSplitViewItem {
        let layerNavigator = LayerNavigatorViewController(project: project, editor: editor)
        self.layerNavigator = layerNavigator
        let navigator = NSSplitViewItem(sidebarWithViewController: layerNavigator)
        navigator.titlebarSeparatorStyle = .none
        navigator.isSpringLoaded = true
        navigator.minimumThickness = Self.minSidebarWidth
        navigator.collapseBehavior = .useConstraints
        return navigator
    }

    private func makeInspector(view: some View) -> NSSplitViewItem {
        let inspector = NSSplitViewItem(inspectorWithViewController: NSHostingController(rootView: view))
        inspector.titlebarSeparatorStyle = .none
        inspector.minimumThickness = Self.minSidebarWidth
        inspector.maximumThickness = 400
//        inspector.collapseBehavior = .useConstraints
        inspector.isSpringLoaded = true
        return inspector
    }

    private func makeCanvas(project: ProjectDocument, editor: AFEditor) -> NSSplitViewItem {
        let canvasViewController = ProjectEditCanvasViewController(project: project, editor: editor)
        self.editCanvas = canvasViewController
        let canvas = NSSplitViewItem(viewController: canvasViewController)
        canvas.titlebarSeparatorStyle = .line
        canvas.minimumThickness = 200
        return canvas
    }

    func saveNavigatorCollapsedState(isCollapsed: Bool) {}

    func saveInspectorCollapsedState(isCollapsed: Bool) {}

    // MARK: - NSSplitViewDelegate

    override func splitView(
        _ splitView: NSSplitView,
        constrainSplitPosition proposedPosition: CGFloat,
        ofSubviewAt dividerIndex: Int
    ) -> CGFloat {
        switch dividerIndex {
        case 0:
            // Navigator
            if (Self.minSnapWidth ... Self.maxSnapWidth).contains(proposedPosition) {
                return Self.snapWidth
            } else if proposedPosition <= Self.minSidebarWidth / 2 {
                hapticCollapse(splitViewItems.first, collapseAction: true)
                return 0
            } else {
                hapticCollapse(splitViewItems.first, collapseAction: false)
                return max(Self.minSidebarWidth, proposedPosition)
            }
        case 1:
            let proposedWidth = view.frame.width - proposedPosition
            if proposedWidth <= Self.minSidebarWidth / 2 {
                hapticCollapse(splitViewItems.last, collapseAction: true)
                return proposedPosition
            } else {
                hapticCollapse(splitViewItems.last, collapseAction: false)
                return min(view.frame.width - Self.minSidebarWidth, proposedPosition)
            }
        default:
            return proposedPosition
        }
    }

    private func hapticCollapse(_ item: NSSplitViewItem?, collapseAction: Bool) {
        if item?.isCollapsed == !collapseAction {
            hapticPerformer.perform(.alignment, performanceTime: .now)
        }
        item?.isCollapsed = collapseAction
    }

    override func splitViewDidResizeSubviews(_ notification: Notification) {
        super.splitViewDidResizeSubviews(notification)
        guard let resizedDivider = notification.userInfo?["NSSplitViewDividerIndex"] as? Int else {
            return
        }

        if resizedDivider == 0 {
            let panel = splitView.subviews[0]
            let width = panel.frame.size.width
            if width > 0 {
//                workspace?.addToWorkspaceState(key: .splitViewWidth, value: width)
            }
        }
    }

    // MARK: ProjectEditCanvasViewControllerDelegate

    func projectEditCanvasViewController(_ controller: ProjectEditCanvasViewController, didNewShape shape: AFLayer) {
        self.layerNavigator?.onAddShape(shape: shape)
    }

    #if DEBUG
        deinit {
            print("\(type(of: self)) deinit")
        }
    #endif
}
