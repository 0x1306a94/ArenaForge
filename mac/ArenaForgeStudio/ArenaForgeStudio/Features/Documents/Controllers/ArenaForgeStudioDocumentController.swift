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
//  ArenaForgeStudioDocumentController.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/10.
//

import arenaforge_editor
import Cocoa
import SwiftUI
import UniformTypeIdentifiers
import WelcomeWindow

final class ArenaForgeStudioDocumentController: NSDocumentController {
    @Environment(\.openWindow)
    private var openWindow
    
    private let fileManager = FileManager.default
    
    @MainActor
    func createAndOpenNewDocument(onCompletion: @escaping () -> Void) {
        guard let newDocumentUrl = self.newDocumentUrl else { return }
        
        if !self.createProjectFile(newDocumentUrl) {
            print("Failed to create new document")
            return
        }
        
        self.openDocument(withContentsOf: newDocumentUrl, display: true) { _, _, _ in
            onCompletion()
        }
    }
    
    func createProjectFile(_ url: URL) -> Bool {
        do {
            let attributes = [
                FileAttributeKey.creationDate: Date(),
            ]
            try self.fileManager.createDirectory(at: url, withIntermediateDirectories: true, attributes: attributes)
            let project = AFProject(name: "test", description: "test", width: 1400, height: 1200)
            let jsonString = project.toJSONString()
            let jsonData = jsonString.data(using: .utf8)
            let projectURL = url.appendingPathComponent("project.json")
            let projectFile = self.fileManager.createFile(atPath: projectURL.path, contents: jsonData, attributes: attributes)
            return projectFile
        } catch {
            return false
        }
    }
    
    override func newDocument(_ sender: Any?) {
        guard let newDocumentUrl = self.newDocumentUrl else { return }
        
        if !self.createProjectFile(newDocumentUrl) {
            print("Failed to create new document")
            return
        }
        
        self.openDocument(withContentsOf: newDocumentUrl, display: true) { _, _, _ in }
    }
    
    private var newDocumentUrl: URL? {
        let panel = NSSavePanel()
        panel.currentContentType = UTType.afproject
        panel.allowedContentTypes = [UTType.afproject]
        guard panel.runModal() == .OK else {
            return nil
        }
        
        return panel.url
    }
    
    override func openDocument(_ sender: Any?) {
        self.openDocument(onCompletion: { document, documentWasAlreadyOpen in
            // TODO: handle errors
            
            guard let document else {
                print("Failed to unwrap document")
                return
            }
            
            print(document, documentWasAlreadyOpen)
        }, onCancel: {})
    }
    
    override func openDocument(
        withContentsOf url: URL,
        display displayDocument: Bool,
        completionHandler: @escaping (NSDocument?, Bool, Error?) -> Void
    ) {
//        guard !openFileInExistingWorkspace(url: url) else {
//            return
//        }
        
        super.openDocument(withContentsOf: url, display: displayDocument) { document, documentWasAlreadyOpen, error in
            if let document {
                self.addDocument(document)
            } else {
                let errorMessage = error?.localizedDescription ?? "unknown error"
                print("Unable to open document '\(url)': \(errorMessage)")
            }
            
            RecentsStore.documentOpened(at: url)
            completionHandler(document, documentWasAlreadyOpen, error)
        }
    }
    
    override func removeDocument(_ document: NSDocument) {
        super.removeDocument(document)
        
        if ArenaForgeStudioDocumentController.shared.documents.isEmpty {
            self.openWindow(sceneID: .welcome)
        }
    }
}

extension NSDocumentController {
    final func openDocument(onCompletion: @escaping (NSDocument?, Bool) -> Void, onCancel: @escaping () -> Void) {
        let dialog = NSOpenPanel()
        
        dialog.title = "Open ArenaForgeStudio Project File"
        dialog.showsResizeIndicator = true
        dialog.showsHiddenFiles = false
        dialog.canChooseFiles = true
        dialog.canChooseDirectories = true
        
        dialog.begin { result in
            if result == NSApplication.ModalResponse.OK, let url = dialog.url {
                self.openDocument(withContentsOf: url, display: true) { document, documentWasAlreadyOpen, error in
                    if let error {
                        NSAlert(error: error).runModal()
                        return
                    }
                    
                    guard let document else {
                        let alert = NSAlert()
                        alert.messageText = NSLocalizedString(
                            "Failed to get document",
                            comment: "Failed to get document"
                        )
                        alert.runModal()
                        return
                    }
                    onCompletion(document, documentWasAlreadyOpen)
                    print("Document:", document)
                    print("Was already open?", documentWasAlreadyOpen)
                }
            } else if result == NSApplication.ModalResponse.cancel {
                onCancel()
            }
        }
    }
}
