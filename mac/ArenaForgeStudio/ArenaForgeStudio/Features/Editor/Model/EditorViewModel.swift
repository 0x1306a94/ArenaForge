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
//  EditorViewModel.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/28.
//

import arenaforge_editor
import SwiftUI

final class EditorViewModel: ObservableObject {
    let editor: AFEditor

    private var hoverTargetLayer: AFLayer?
    private var hoverWireframeLayer: AFLayer?

    init(editor: AFEditor) {
        self.editor = editor
    }
    
    func findLayer(at point: CGPoint) -> AFLayer? {
        editor.findLayer(at: point)
    }

    func clearHoverWireframe() {
        hoverTargetLayer = nil
        editor.resetHoverWireframe()
    }

    func createHoverWireframeLayer(targetLayer: AFLayer) {
        if let hoverTargetLayer, targetLayer == hoverTargetLayer {
            return
        }

        clearHoverWireframe()
        hoverTargetLayer = targetLayer
        editor.createHoverWireframeLayer(inTargetLayer: targetLayer)
    }
}
