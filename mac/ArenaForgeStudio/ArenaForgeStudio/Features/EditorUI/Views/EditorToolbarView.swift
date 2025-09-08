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
//  EditorMouseModeToolbarButton.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/8.
//

import SwiftUI

struct EditorToolbarView: View {
    @Environment(\.controlActiveState)
    private var activeState

    @EnvironmentObject var workspace: WorkspaceDocument

    var body: some View {
        HStack(spacing: 4) {
            ForEach(EditorToolbarItem.allCases, id: \.self) { item in
                EditorToolbarButton(item: item, isSelected: workspace.activateEditorToolbarItem == item) {
                    workspace.activateEditorToolbarItem = item
                }
            }
        }
        .fixedSize(horizontal: false, vertical: false)
        .clipped()
    }
}

struct EditorToolbarButton: View {
    @Environment(\.controlActiveState)
    private var controlActive

    let item: EditorToolbarItem
    let isSelected: Bool
    let action: () -> Void

    @State private var isHovering: Bool = false

    var body: some View {
        HStack {
            Button {
                action()
            } label: {
                Image(item.iconName)
                    .resizable()
                    .renderingMode(.template)
                    .foregroundColor(isSelected ? Color.white : Color.secondary)
                    .frame(width: 24, height: 24)
                    .opacity(controlActive == .inactive ? 0.5 : 1.0)
                    .help(item.helpTip)
            }
        }
    }
}
