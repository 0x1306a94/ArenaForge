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
//  ArenaForgeStudioApp.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/7.
//

import SwiftUI
import WelcomeWindow

@main
struct ArenaForgeStudioApp: App {
    @NSApplicationDelegateAdaptor var appdelegate: AppDelegate

    init() {
        _ = ArenaForgeStudioDocumentController.shared
    }

    var body: some Scene {
        Group {
            WelcomeWindow(
                // Add two action buttons below your icon
                actions: { dismissWindow in
                    NewProjectButton(dismissWindow: dismissWindow)
                    OpenProjectButton(dismissWindow: dismissWindow)
                },
                // Receive files via drag and drop
                onDrop: { url, dismiss in
                    print("File dropped at: \(url.path)")

                    dismiss()
                }
            )
        }
        .commands {
            CommandGroup(replacing: .newItem) {
                Button("New Project") {
                    ArenaForgeStudioDocumentController.shared.newDocument(nil)
                }
                .keyboardShortcut("n")
            }

            CommandGroup(replacing: .saveItem) {
                Button("Save") {
                    NSDocumentController.shared.currentDocument?.save(nil)
                }
                .keyboardShortcut("s")
            }

            CommandGroup(after: .saveItem) {
                Button("Save As…") {
                    if let doc = NSDocumentController.shared.currentDocument {
                        doc.runModalSavePanel(for: .saveAsOperation, delegate: nil, didSave: nil, contextInfo: nil)
                    }
                }
                .keyboardShortcut("S", modifiers: [.shift, .command])
            }
        }
    }
}
