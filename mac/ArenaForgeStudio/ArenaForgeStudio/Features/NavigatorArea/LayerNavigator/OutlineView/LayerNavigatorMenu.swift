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

        guard document != nil else {
            return
        }

        if selectedLayers.isEmpty {
            return
        }

        let totalParenCount = selectedLayers.filter { $0.parent != nil }.count
        if selectedLayers.count > 1, totalParenCount == selectedLayers.count {
            let venues = Set(selectedLayers.compactMap { $0.attachVenue })
            let parents = Set(selectedLayers.compactMap { $0.parent })
            if venues.count == 1, parents.count == 1 {
                let groupItem = menuItem("Group", action: #selector(upgradeGroup))
                items.append(groupItem)
            }
        } else if selectedLayers.count == 1, selectedLayers[0].type == .group {
            let groupItem = menuItem("Undo Group", action: #selector(undoGroup))
            items.append(groupItem)
        } else {
            if let layer = selectedLayers.first, layer.venue == nil {
                let duplicateItem = menuItem("Duplicate", action: #selector(duplicate))
                items.append(duplicateItem)
            }
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
        guard let project = document?.project else {
            return
        }

        let allLayers = Set(layers)

        // 先找出涉及到的 venues
        let venues = Set(allLayers.compactMap { $0.attachVenue })

        // 其中根节点在删除集合中的 venue
        let venuesToDelete = venues.filter { venue in
            allLayers.contains { $0 === venue.root }
        }

        let deletedVenueInfo: [(venue: AFVenue, index: Int32)] = venuesToDelete.map {
            ($0, project.getVenueIndex($0))
        }
        .sorted { $0.index < $1.index }

        // 删除整个 venue
        for item in venuesToDelete {
            document?.project?.removeVenue(item)
        }

        // 过滤掉已经整体删除 venue 的那些 layer
        var remainder = allLayers.filter { layer in
            guard let venue = layer.attachVenue else { return true }
            return !venuesToDelete.contains(venue)
        }

        // 只保留没有 parent 在 remainder 中的顶层 layer
        remainder = remainder.filter { layer in
            guard let parent = layer.parent else { return true }
            return !remainder.contains(parent)
        }

        // 删除Layer前记录 parent 和 index
        let deletedLayerInfo: [(layer: AFLayer, parent: AFLayer, index: Int32)] = remainder.compactMap { layer in
            guard let parent = layer.parent else { return nil }
            let idx = parent.getChildIndex(layer)
            return (layer, parent, idx)
        }
        .sorted { $0.index < $1.index }

        // 删除剩余的普通 layer
        remainder.forEach { $0.removeFromParent() }

        document?.undoManager?.registerUndo(withTarget: self) { [weak self] _ in
            guard let document = self?.document, let project = document.project else {
                return
            }

            for deletedVenueInfo in deletedVenueInfo {
                project.addVenue(deletedVenueInfo.venue, at: deletedVenueInfo.index)
            }

            for deletedLayerInfo in deletedLayerInfo {
                deletedLayerInfo.parent.addChild(deletedLayerInfo.layer, at: deletedLayerInfo.index)
            }

            self?.reloadData()
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
        guard !layers.isEmpty else {
            return
        }

        let venues = Set(layers.compactMap { $0.attachVenue })
        guard venues.count == 1, let veune = venues.first else {
            return
        }

        if let group = veune.upgradeGroup(layers) {
            document?.undoManager?.registerUndo(withTarget: self) { [weak veune, weak group] in
                guard let veune, let group else { return }
                $0.performUndoGroup(veune: veune, group: group)
            }

            sender?.outlineView.reloadItem(veune.root, reloadChildren: true)
        }
    }

    @objc
    func undoGroup() {
        guard selectedLayers.count == 1 else {
            return
        }
        let venues = Set(selectedLayers.compactMap { $0.attachVenue })
        guard venues.count == 1, let veune = venues.first else {
            return
        }

        let group = selectedLayers[0]
        performUndoGroup(veune: veune, group: group)
    }

    private func performUndoGroup(veune: AFVenue, group: AFLayer) {
        guard veune.undoGroup(group) else {
            return
        }
        document?.undoManager?.registerUndo(withTarget: self) { [weak group] in
            guard let group else {
                return
            }
            $0.performUpgradeGroup(layers: group.children)
        }

        sender?.outlineView.reloadItem(veune.root, reloadChildren: true)
    }
}
