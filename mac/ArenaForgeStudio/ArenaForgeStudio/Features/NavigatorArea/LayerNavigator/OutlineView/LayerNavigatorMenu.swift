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
//  LayerNavigatorMenu.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/11.
//

import AppKit
import arenaforge_editor

final class LayerNavigatorMenu: NSMenu {
    private weak var document: ProjectDocument?
    private weak var sender: LayerNavigatorViewController?

    var selectedLayers: [AFLayer] = []

    init(_ sender: LayerNavigatorViewController, document: ProjectDocument?) {
        self.sender = sender
        self.document = document
        super.init(title: "Options")
    }

    @available(*, unavailable)
    required init(coder _: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    private func menuItem(_ title: String, action: Selector?, key: String = "") -> NSMenuItem {
        let mItem = NSMenuItem(title: title, action: action, keyEquivalent: key)
        mItem.target = self

        return mItem
    }

    private func setupMenu() {
        removeAllItems()

        guard document != nil else { return }

        if selectedLayers.isEmpty { return }

        let parents = selectedLayers.compactMap { $0.parent }
        let parentsSet = Set(parents)
        if selectedLayers.count > 1, parents.count == selectedLayers.count, parentsSet.count == 1 {
            let groupItem = menuItem("Group", action: #selector(upgradeGroup))
            items.append(groupItem)
        } else if selectedLayers.count == 1, selectedLayers[0].type == .group {
            let groupItem = menuItem("Undo Group", action: #selector(undoGroup))
            items.append(groupItem)
        } else {
            let duplicateItem = menuItem("Duplicate", action: #selector(duplicate))
            items.append(duplicateItem)
        }

        let deleteItem = menuItem("Delete", action: #selector(delete))
        items.append(deleteItem)
    }

    func reset() {
        selectedLayers = []
    }

    override func update() {
        setupMenu()
    }

    private func reloadData() {
        sender?.outlineView.reloadData()
    }
}

// MARK: Actions

extension LayerNavigatorMenu {
    @objc
    func delete() {
        performDelete(layers: selectedLayers)
    }

    private func performDelete(layers: [AFLayer]) {
        guard let project = document?.project else { return }

        let deletedLayerInfo: [(parent: AFLayer, child: AFLayer, index: Int32)] = layers.compactMap { child in
            guard let parent = child.parent else { return nil }
            let index = parent.getChildIndex(child)
            return (parent, child, index)
        }
        .sorted { $0.index < $1.index }

        guard !deletedLayerInfo.isEmpty else { return }

        for deletedLayerInfo in deletedLayerInfo {
            deletedLayerInfo.parent.removeChild(deletedLayerInfo.child)
        }

        document?.undoManager?.registerUndo(withTarget: self) {
            for deletedLayerInfo in deletedLayerInfo {
                deletedLayerInfo.parent.addChild(deletedLayerInfo.child, at: deletedLayerInfo.index)
            }

            $0.reloadData()
        }

        reloadData()
    }

    @objc
    func duplicate() {}

    @objc
    func upgradeGroup() {
        performUpgradeGroup(layers: selectedLayers)
    }

    private func performUpgradeGroup(layers: [AFLayer]) {
        guard !layers.isEmpty else { return }

        guard let project = document?.project else { return }

        guard let group = project.upgradeGroup(layers) else { return }

        document?.undoManager?.registerUndo(withTarget: self, handler: { [weak group] in
            guard let group else { return }
            $0.performUndoGroup(group: group)
        })

        reloadData()
    }

    @objc
    func undoGroup() {
        guard selectedLayers.count == 1 else { return }

        let group = selectedLayers[0]
        performUndoGroup(group: group)
    }

    private func performUndoGroup(group: AFLayer) {
        guard let project = document?.project else { return }

        guard project.undoGroup(group) else { return }

        document?.undoManager?.registerUndo(withTarget: self, handler: { [weak group] in
            guard let group else { return }
            $0.performUpgradeGroup(layers: group.children)
        })

        reloadData()
    }
}
