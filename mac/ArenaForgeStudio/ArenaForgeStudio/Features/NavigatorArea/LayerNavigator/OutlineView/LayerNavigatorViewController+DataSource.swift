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
        return venues.count
    }

    func outlineView(_ outlineView: NSOutlineView, child index: Int, ofItem item: Any?) -> Any {
        if let layer = item as? AFLayer {
            let reversedIndex = layer.childrenCount - 1 - index
            return layer.children[reversedIndex]
        }

        let reversedIndex = venues.count - 1 - index
        return venues[reversedIndex].root
    }

    func outlineView(_ outlineView: NSOutlineView, isItemExpandable item: Any) -> Bool {
        if let layer = item as? AFLayer {
            return layer.hasChildren
        }

        return false
    }

    func outlineView(_ outlineView: NSOutlineView, itemForPersistentObject object: Any) -> Any? {
        guard let layerId = object as? String else { return nil }
        for venue in self.venues {
            if layerId == venue.venueId {
                return venue.root
            } else {
                if let layer = venue.findLayer(byId: layerId) {
                    return layer
                }
            }
        }

        return nil
    }

    func outlineView(_ outlineView: NSOutlineView, persistentObjectForItem item: Any?) -> Any? {
        guard let layer = item as? AFLayer else { return nil }
        if let venue = layer.venue {
            return venue.venueId
        }
        return layer.layerId
    }
}
