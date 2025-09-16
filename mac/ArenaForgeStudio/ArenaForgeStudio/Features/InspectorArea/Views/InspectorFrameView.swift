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
//  InspectorFrameView.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/16.
//

import SwiftUI

struct InspectorFrameView: View {
    @State private var x: Double = 0
    @State private var y: Double = 0
    @State private var width: Double = 0
    @State private var height: Double = 0

    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            Text("Frame")
                .font(.subheadline)
                .foregroundStyle(.iconSecondary)

            HStack(spacing: 6) {
                DecimalInputField(label: "X", initialValue: x) { newValue, reason in
                    print("X: \(newValue) \(reason)")
                }

                DecimalInputField(label: "Y", initialValue: y) { newValue, reason in
                    print("Y: \(newValue) \(reason)")
                }
            }

            HStack(spacing: 6) {
                DecimalInputField(label: "W", initialValue: width) { newValue, reason in
                    print("W: \(newValue) \(reason)")
                }

                DecimalInputField(label: "H", initialValue: height) { newValue, reason in
                    print("H: \(newValue) \(reason)")
                }
            }
        }
        .padding(.horizontal, 10)
        .frame(maxWidth: .infinity)
    }
}

#Preview {
    InspectorFrameView()
}
