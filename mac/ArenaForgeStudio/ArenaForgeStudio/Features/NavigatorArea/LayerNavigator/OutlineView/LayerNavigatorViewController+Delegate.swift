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
//  LayerNavigatorViewController+Delegate.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/7.
//

import AppKit
import arenaforge_editor

extension LayerNavigatorViewController: NSOutlineViewDelegate {
    func outlineView(
        _ outlineView: NSOutlineView,
        shouldShowCellExpansionFor tableColumn: NSTableColumn?,
        item: Any
    ) -> Bool {
        true
    }

    func outlineView(_ outlineView: NSOutlineView, shouldShowOutlineCellForItem item: Any) -> Bool {
        true
    }

    func outlineView(_ outlineView: NSOutlineView, heightOfRowByItem item: Any) -> CGFloat {
        rowHeight // This can be changed to 20 to match Xcode's row height.
    }

    func outlineView(_ outlineView: NSOutlineView, viewFor tableColumn: NSTableColumn?, item: Any) -> NSView? {
        guard let tableColumn else { return nil }

        let frameRect = NSRect(x: 0, y: 0, width: tableColumn.width, height: rowHeight)
        let cell = LayerTableViewCell(
            frame: frameRect,
            item: item as? AFLayer
        )
        cell.delegate = self
        return cell
    }

    func outlineViewSelectionDidChange(_ notification: Notification) {
        guard let outlineView = notification.object as? NSOutlineView, outlineView == self.outlineView else { return }

        guard let userInfo = notification.userInfo else { return }
        guard let currentSelectIndexSet = userInfo["NSTableViewCurrentRowSelectionUserInfoKey"] as? IndexSet, let previousSelectIndexSet = userInfo["NSTableViewPreviousRowSelectionUserInfoKey"] as? IndexSet else {
            return
        }

        print(previousSelectIndexSet.map { $0 })
        print(currentSelectIndexSet.map { $0 })
//        guard let item = outlineView.item(atRow: selectedIndex) as? AFLayer else { return }
    }
}

extension LayerNavigatorViewController: LayerTableViewCellDelegate {
    func layerTableViewCellNameDidFinishEditing(_ cell: LayerTableViewCell, newname: String) {
        guard let item = cell.layerItem else {
            return
        }

        let oldName: String
        if let veune = item.venue {
            oldName = veune.name
            veune.name = newname
        } else {
            oldName = item.name
            item.name = newname
        }

        self.project?.undoManager?.registerUndo(withTarget: self) { [weak self, weak item] _ in
            guard let self, let item else {
                return
            }

            if let veune = item.venue {
                veune.name = oldName
            } else {
                item.name = oldName
            }
            self.outlineView.reloadItem(item)
        }

        self.outlineView.reloadItem(item)
    }
}
