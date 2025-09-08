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
//  WorkspaceDocument.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/7.
//

import AppKit
import SwiftUI
import Combine
import arenaforge_editor

final class WorkspaceDocument: NSDocument, ObservableObject {
    
    @Published var activateEditorToolbarItem: EditorToolbarItem = .cursors
    
    var project: AFProject?

    override static var autosavesInPlace: Bool {
        false
    }

    override var isDocumentEdited: Bool {
        false
    }

    override func makeWindowControllers() {
        let window = NSWindow(
            contentRect: NSRect(x: 0, y: 0, width: 1400, height: 900),
            styleMask: [.titled, .closable, .miniaturizable, .resizable, .fullSizeContentView],
            backing: .buffered,
            defer: false
        )

        window.minSize = NSSize(width: 1000, height: 600)

        let windowController = WorkspaceEditWindowController(window: window, workspace: self)

        window.setFrame(NSRect(x: 0, y: 0, width: 1400, height: 900), display: true, animate: false)
        window.center()

        window.setAccessibilityIdentifier("workspace")
        window.setAccessibilityDocument(self.fileURL?.absoluteString)

        self.addWindowController(windowController)

        window.makeKeyAndOrderFront(nil)
    }

    private func initWorkspaceState(_ url: URL) throws {
        var url = url
        if !url.absoluteString.hasSuffix("/") {
            url = URL(filePath: url.absoluteURL.path(percentEncoded: false) + "/")
        }

        self.fileURL = url
        self.displayName = url.lastPathComponent

        let project = try AFProject(fileURL: url)
        self.project = project
    }

    override func read(from url: URL, ofType typeName: String) throws {
        try self.initWorkspaceState(url)
    }

    override func write(to url: URL, ofType typeName: String) throws {}

    // MARK: Close Workspace

    override func close() {
        super.close()
    }

    #if DEBUG
        deinit {
            Swift.print("\(type(of: self)) deinit")
        }
    #endif
}
