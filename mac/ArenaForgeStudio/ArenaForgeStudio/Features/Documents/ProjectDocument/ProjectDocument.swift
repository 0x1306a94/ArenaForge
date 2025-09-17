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
//  ProjectDocument.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/7.
//

import AppKit
import arenaforge_editor
import Combine
import SwiftUI

final class ProjectDocument: NSDocument, ObservableObject {
    @Published var activateEditorToolbarItem: EditorToolbarItem = .cursors
    
    var project: AFProject?
    
    override static var autosavesInPlace: Bool {
        false
    }
    
    // MARK: - Window setup

    override func makeWindowControllers() {
        let window = NSWindow(
            contentRect: NSRect(x: 0, y: 0, width: 1400, height: 900),
            styleMask: [.titled, .closable, .miniaturizable, .resizable, .fullSizeContentView],
            backing: .buffered,
            defer: false
        )
        
        window.minSize = NSSize(width: 1000, height: 600)
        
        let windowController = ProjectEditWindowController(window: window, project: self)
        
        window.setFrame(NSRect(x: 0, y: 0, width: 1400, height: 900), display: true, animate: false)
        window.center()
        
        window.setAccessibilityIdentifier("project")
        window.setAccessibilityDocument(self.fileURL?.absoluteString)
        
        self.addWindowController(windowController)
        
        window.makeKeyAndOrderFront(nil)
    }
    
    // MARK: - FileWrapper API

    override func read(from fileWrapper: FileWrapper, ofType typeName: String) throws {
        guard let fileURL else {
            throw NSError(domain: "ProjectDocument", code: 1, userInfo: [
                NSLocalizedDescriptionKey: "Missing project.json in package"
            ])
        }
        
        self.project = try AFProject(fileURL: fileURL)
        self.displayName = self.fileURL?.lastPathComponent ?? "Untitled"
    }
    
    override func fileWrapper(ofType typeName: String) throws -> FileWrapper {
        let rootWrapper = FileWrapper(directoryWithFileWrappers: [:])
        
        guard let project else { return rootWrapper }
        
        let projectJSON = project.toJSONString()
        guard let projectData = projectJSON.data(using: .utf8) else {
            return rootWrapper
        }
        
        let projectFile = FileWrapper(regularFileWithContents: projectData)
        projectFile.preferredFilename = "project.json"
        rootWrapper.addFileWrapper(projectFile)

        return rootWrapper
    }
    
    // MARK: Close Project

    override func close() {
        super.close()
    }
    
    #if DEBUG
        deinit {
            Swift.print("\(type(of: self)) deinit")
        }
    #endif
}
