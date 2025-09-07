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
//  LayerNavigatorViewController+DataSource.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/7.
//

import AppKit

extension LayerNavigatorViewController: NSOutlineViewDataSource {
//    private func getOutlineViewItems(for item: any) -> [any] {
//        return []
//    }

    func outlineView(_ outlineView: NSOutlineView, numberOfChildrenOfItem item: Any?) -> Int {
//        if let item = item as? CEWorkspaceFile {
//            return getOutlineViewItems(for: item).count
//        }
//        return content.count
        return 0
    }

    func outlineView(_ outlineView: NSOutlineView, child index: Int, ofItem item: Any?) -> Any {
//        if let item = item as? CEWorkspaceFile {
//            return getOutlineViewItems(for: item)[index]
//        }
//        return content[index]
        return 0
    }

    func outlineView(_ outlineView: NSOutlineView, isItemExpandable item: Any) -> Bool {
//        if let item = item as? CEWorkspaceFile {
//            return item.isFolder
//        }
        return false
    }
}
