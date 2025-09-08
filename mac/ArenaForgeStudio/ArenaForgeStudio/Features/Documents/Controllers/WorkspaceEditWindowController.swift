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
//  WorkspaceEditWindowController.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/7.
//

import AppKit
import arenaforge_editor
import Combine

final class WorkspaceEditWindowController: NSWindowController, NSWindowDelegate, ObservableObject {
    @Published var navigatorCollapsed: Bool = false
    @Published var inspectorCollapsed: Bool = false

    var workspce: WorkspaceDocument?
    var editor: AFEditor?

    var splitViewController: WorkspaceEditSplitViewController? {
        contentViewController as? WorkspaceEditSplitViewController
    }

    init(window: NSWindow?, workspace: WorkspaceDocument?) {
        super.init(window: window)
        window?.delegate = self
        guard let workspace else { return }
        self.workspce = workspace

        guard let editor = makeEditor(workspace: workspace) else {
            fatalError("Failed to set up editor.")
        }

        self.editor = editor

        guard let splitViewController = setupSplitViewController(workspace: workspace, editor: editor) else {
            fatalError("Failed to set up content view.")
        }

        contentViewController = splitViewController

        setupToolbar()
    }

    @available(*, unavailable)
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    override func windowDidLoad() {
        super.windowDidLoad()
    }

    func setupSplitViewController(workspace: WorkspaceDocument, editor: AFEditor) -> WorkspaceEditSplitViewController? {
        guard let window else {
            assertionFailure("No window found for this controller. Cannot set up content.")
            return nil
        }
        return WorkspaceEditSplitViewController(windowRef: window, workspace: workspace, editor: editor)
    }

    private func makeEditor(workspace: WorkspaceDocument) -> AFEditor? {
        guard let project = workspace.project else {
            return nil
        }
        let editor = AFEditor(project: project)
        return editor
    }

    // MARK: NSWindowDelegate

    func windowShouldClose(_ sender: NSWindow) -> Bool {
        return true
    }

    #if DEBUG
        deinit {
            print("\(type(of: self)) deinit")
        }
    #endif
}
