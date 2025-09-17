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
import arenaforge_editor

extension LayerNavigatorViewController: NSOutlineViewDataSource {
    func outlineView(_ outlineView: NSOutlineView, numberOfChildrenOfItem item: Any?) -> Int {
        if let layer = item as? AFLayer {
            return layer.childrenCount
        }
        return layers.count
    }

    func outlineView(_ outlineView: NSOutlineView, child index: Int, ofItem item: Any?) -> Any {
        if let layer = item as? AFLayer {
            let reversedIndex = layer.childrenCount - 1 - index
            return layer.children[reversedIndex]
        }

        let reversedIndex = layers.count - 1 - index
        return layers[reversedIndex]
    }

    func outlineView(_ outlineView: NSOutlineView, isItemExpandable item: Any) -> Bool {
        if let layer = item as? AFLayer {
            return layer.hasChildren
        }

        return false
    }

    func outlineView(_ outlineView: NSOutlineView, itemForPersistentObject object: Any) -> Any? {
        guard let project, let layerId = object as? String else { return nil }
        if let layer = project.project?.findLayer(byId: layerId) {
            return layer
        }

        return nil
    }

    func outlineView(_ outlineView: NSOutlineView, persistentObjectForItem item: Any?) -> Any? {
        guard let layer = item as? AFLayer else { return nil }
        return layer.layerId
    }

    func outlineView(_ outlineView: NSOutlineView, pasteboardWriterForItem item: Any) -> NSPasteboardWriting? {
        guard let layer = item as? AFLayer else { return nil }
        let pbItem = NSPasteboardItem()
        pbItem.setString(layer.layerId, forType: .string) // 用 layerId 作为唯一标识
        return pbItem
    }

    func outlineView(_ outlineView: NSOutlineView, validateDrop info: any NSDraggingInfo, proposedItem item: Any?, proposedChildIndex index: Int) -> NSDragOperation {
        guard item is AFLayer else {
            return []
        }

        // 如果 index == -1，表示要放到 parent 节点本身
        guard index != -1 else { return [] }

        return .move
    }

    func outlineView(_ outlineView: NSOutlineView, acceptDrop info: NSDraggingInfo, item: Any?, childIndex index: Int) -> Bool {
        guard let destination = item as? AFLayer else { return false }
        let pb = info.draggingPasteboard
        guard let layerId = pb.string(forType: .string) else { return false }

        // 找到拖动的 layer
        guard let source = project?.project?.findLayer(byId: layerId), let sourceParent = source.parent else { return false }

        // 只允许同级内
        let parents = Set([destination, source].compactMap { $0.parent })
        if parents.count != 1 {
            return false
        }

        let oldIndex = sourceParent.getChildIndex(source)
        let oldFrame = source.frame
        let oldGlobal = source.local(toGlobal: .zero)

        // 显示是按照倒序
        var reversedIndex = destination.childrenCount - index
        if reversedIndex < 0 {
            reversedIndex = 0
        } else if index == 0 {
            reversedIndex = destination.childrenCount
        }

        guard destination.addChild(source, at: Int32(reversedIndex)) else {
            return false
        }

        if sourceParent != destination {
            let newLocal = destination.global(toLocal: oldGlobal)
            var newFrame = oldFrame
            newFrame.origin = newLocal
            source.frame = newFrame
        }

        self.project?.undoManager?.registerUndo(withTarget: self) { [weak sourceParent, weak source, weak self] _ in
            guard let sourceParent, let source, let self else {
                return
            }

            guard sourceParent.addChild(source, at: oldIndex) else {
                return
            }
            source.frame = oldFrame
            self.outlineView.reloadData()
        }

        outlineView.reloadData()
        return false
    }
}
