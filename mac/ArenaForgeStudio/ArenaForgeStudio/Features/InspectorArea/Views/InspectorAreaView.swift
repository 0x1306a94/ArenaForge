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
//  InspectorAreaView.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/7.
//

import SwiftUI

struct InspectorAreaView: View {
    var body: some View {
        VStack(spacing: 0) {
            Text("Hello, World!")
        }
        .safeAreaInset(edge: .leading, spacing: 0) {
//            if sidebarPosition == .side {
//                HStack(spacing: 0) {
//                    WorkspacePanelTabBar(items: $tabItems, selection: $selectedTab, position: sidebarPosition)
//                    Divider()
//                        .overlay(Color(nsColor: darkDivider && colorScheme == .dark ? .black : .clear))
//                }
//            }
        }
        .safeAreaInset(edge: .top, spacing: 0) {
//            if sidebarPosition == .top {
//                VStack(spacing: 0) {
//                    Divider()
//                    WorkspacePanelTabBar(items: $tabItems, selection: $selectedTab, position: sidebarPosition)
//                    Divider()
//                }
//            } else if !darkDivider {
//                Divider()
//            }
        }
    }
}

#Preview {
    InspectorAreaView()
}
