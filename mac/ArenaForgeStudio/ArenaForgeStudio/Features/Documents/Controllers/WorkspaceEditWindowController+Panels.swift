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
//  WorkspaceEditWindowController+Panels.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/7.
//

import AppKit

extension WorkspaceEditWindowController {
    @objc
    func objcToggleFirstPanel() {
        toggleFirstPanel(shouldAnimate: true)
    }

    func toggleFirstPanel(shouldAnimate: Bool = true) {
        guard let firstSplitView = splitViewController?.splitViewItems.first else { return }

        if shouldAnimate {
            firstSplitView.animator().isCollapsed.toggle()
        } else {
            firstSplitView.isCollapsed.toggle()
        }

        splitViewController?.saveNavigatorCollapsedState(isCollapsed: firstSplitView.isCollapsed)
    }

    @objc
    func objcToggleLastPanel() {
        toggleLastPanel(shouldAnimate: true)
    }

    func toggleLastPanel(shouldAnimate: Bool = true) {
        guard let lastSplitView = splitViewController?.splitViewItems.last else {
            return
        }

        if shouldAnimate {
            NSAnimationContext.runAnimationGroup { _ in
                lastSplitView.animator().isCollapsed.toggle()
            }
        } else {
            lastSplitView.isCollapsed.toggle()
        }

        splitViewController?.saveInspectorCollapsedState(isCollapsed: lastSplitView.isCollapsed)
    }
}
