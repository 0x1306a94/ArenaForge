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
//  LayerNavigatorViewController+MenuDelegate.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/11.
//

import AppKit
import arenaforge_editor

extension LayerNavigatorViewController: NSMenuDelegate {
    func menuNeedsUpdate(_ menu: NSMenu) {
        let clickedRow = self.outlineView.clickedRow
        guard let menu = menu as? LayerNavigatorMenu else {
            return
        }

        menu.reset()
        guard clickedRow != -1 else {
            menu.update()
            return
        }

        let selectedLayers = outlineView.selectedRowIndexes.compactMap {
            outlineView.item(atRow: $0) as? AFLayer
        }

        guard let item = outlineView.item(atRow: clickedRow) as? AFLayer else {
            menu.update()
            return
        }

        if selectedLayers.count > 1 {
            if selectedLayers.contains(item) {
                menu.selectedLayers = selectedLayers
                menu.update()
                return
            }
        }

        menu.selectedLayers = [item]
        menu.update()
    }
}
