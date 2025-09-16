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
//  InspectorAlignmentView.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/16.
//

import SwiftUI

struct InspectorAlignmentView: View {
    enum HorizontalAlignment: String, CaseIterable {
        case left, center, right

        var systemImage: String {
            switch self {
            case .left: return "align.horizontal.left.fill"
            case .center: return "align.horizontal.center.fill"
            case .right: return "align.horizontal.right.fill"
            }
        }

        var accessibilityLabel: String {
            switch self {
            case .left: return "Left Alignment"
            case .center: return "Center Alignment"
            case .right: return "Right Alignment"
            }
        }
    }

    enum VerticalAlignment: String, CaseIterable {
        case top, center, bottom

        var systemImage: String {
            switch self {
            case .top: return "align.vertical.top.fill"
            case .center: return "align.vertical.center.fill"
            case .bottom: return "align.vertical.bottom.fill"
            }
        }

        var accessibilityLabel: String {
            switch self {
            case .top: return "Top Alignment"
            case .center: return "Center Alignment"
            case .bottom: return "Bottom Alignment"
            }
        }
    }

    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            Text("Alignment")
                .font(.subheadline)
                .foregroundStyle(.iconSecondary)

            HStack(spacing: 6) {
                HStack(spacing: 1) {
                    ForEach(HorizontalAlignment.allCases.indices, id: \.self) { index in
                        Button {} label: {
                            Image(systemName: HorizontalAlignment.allCases[index].systemImage)
                                .resizable()
                                .frame(width: 12, height: 12)
                                .foregroundStyle(.icon)
                        }
                        .buttonStyle(.plain)
                        .frame(height: 24)
                        .frame(maxWidth: .infinity)
                        .background(.bgSecondary)
                        .clipShape(clipShape(index: index, count: HorizontalAlignment.allCases.count))
                    }
                }

                HStack(spacing: 1) {
                    ForEach(VerticalAlignment.allCases.indices, id: \.self) { index in
                        Button {} label: {
                            Image(systemName: VerticalAlignment.allCases[index].systemImage)
                                .resizable()
                                .frame(width: 12, height: 12)
                                .foregroundStyle(.icon)
                        }
                        .buttonStyle(.plain)
                        .frame(height: 24)
                        .frame(maxWidth: .infinity)
                        .background(.bgSecondary)
                        .clipShape(clipShape(index: index, count: VerticalAlignment.allCases.count))
                    }
                }
            }
            .frame(maxWidth: .infinity)
        }
        .padding(.horizontal, 10)
    }

    func clipShape(index: Int, count: Int) -> some Shape {
        if index == 0 {
            UnevenRoundedRectangle(cornerRadii: .init(topLeading: 4, bottomLeading: 4))
        } else if index == count - 1 {
            UnevenRoundedRectangle(cornerRadii: .init(bottomTrailing: 4, topTrailing: 4))
        } else {
            UnevenRoundedRectangle(cornerRadii: .init())
        }
    }
}

#Preview {
    InspectorAlignmentView()
}
