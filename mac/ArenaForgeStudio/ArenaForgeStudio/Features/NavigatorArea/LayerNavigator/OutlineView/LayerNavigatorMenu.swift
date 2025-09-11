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

        if selectedLayers.count >= 1 {
            let venues = Set(selectedLayers.compactMap { $0.attachVenue })
            if venues.count == 1 {
                let groupItem = menuItem("Group", action: #selector(upgradeGroup))
                items.append(groupItem)
            }
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
        for layer in selectedLayers {
            if let veune = layer.venue {
                // root
                document?.project?.removeVenue(veune)
            } else {
                layer.removeFromParent()
            }
        }

        reloadData()
    }

    @objc
    func duplicate() {}

    @objc
    func upgradeGroup() {
        let venues = Set(selectedLayers.compactMap { $0.attachVenue })
        guard venues.count == 1, let veune = venues.first else {
            return
        }

        if let _ = veune.upgradeGroup(selectedLayers) {
            sender?.outlineView.reloadItem(veune.root, reloadChildren: true)
        }
    }
}
