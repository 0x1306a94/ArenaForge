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
//  DecimalInputField.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/16.
//

import SwiftUI

struct DecimalInputField: View {
    enum ValueChangeReason {
        case enterKey
        case focusLost
        case sliderBegin
        case slider
        case sliderEnd
    }
    
    private let label: String
    private let initialValue: Double
    private let maximumFractionDigits: Int
    private let formatter: NumberFormatter
    @State private var sliderReason: ValueChangeReason = .sliderEnd
    
    @State private var value: Double
    @State private var dragAccumulator: CGFloat = 0
    @FocusState private var isFocused: Bool
    
    typealias ValueChangedHandler = (Double, ValueChangeReason) -> Void
    let onValueChanged: ValueChangedHandler?
    
    init(label: String, initialValue: Double, maximumFractionDigits: Int = 2, onValueChanged: ValueChangedHandler? = nil) {
        self.label = label
        self.initialValue = initialValue
        self.maximumFractionDigits = maximumFractionDigits
        self._value = State(initialValue: initialValue)
        self.onValueChanged = onValueChanged
        
        self.formatter = NumberFormatter()
        formatter.usesGroupingSeparator = false
        formatter.minimumFractionDigits = 0
        formatter.maximumFractionDigits = maximumFractionDigits
        formatter.numberStyle = .decimal
    }
    
    var body: some View {
        HStack(spacing: 0) {
            Text(label)
                .font(.caption)
                .foregroundStyle(.iconSecondary)
                .frame(width: 24, alignment: .center)
                .contentShape(Rectangle())
                .gesture(
                    DragGesture(minimumDistance: 0)
                        .onChanged { gesture in
                            let deltaX = gesture.translation.width
                            let step = Int(deltaX * 0.1 - dragAccumulator)
                            if step != 0 {
                                dragAccumulator += CGFloat(step)
                                value += Double(step)
                                if sliderReason == .sliderEnd {
                                    sliderReason = .sliderBegin
                                } else if sliderReason == .sliderBegin {
                                    sliderReason = .slider
                                }
                                
                                onValueChanged?(value, sliderReason)
                            }
                        }
                        .onEnded { _ in
                            dragAccumulator = 0
                            sliderReason = .sliderEnd
                            onValueChanged?(value, sliderReason)
                         }
                )
            
            TextField("", value: $value, formatter: formatter)
                .textFieldStyle(.plain)
                .multilineTextAlignment(.leading)
                .focused($isFocused)
                .frame(maxWidth: .infinity)
                .onSubmit {
                    onValueChanged?(value, .enterKey)
                }
//                .onChange(of: isFocused) { oldValue, newValue in
//                    if oldValue, !newValue, value != initialValue {
//                        onValueChanged?(value, .focusLost)
//                    }
//                }
        }
        .frame(height: 24)
        .background(
            RoundedRectangle(cornerRadius: 4)
                .fill(.bgSecondary)
        )
        .onChange(of: initialValue) { _, newValue in
            value = newValue
        }
    }
}

#Preview {
    @Previewable var x: Double = 0
    
    DecimalInputField(label: "X", initialValue: x) { newValue, reason in
        print("X: \(newValue) \(reason)")
    }
}
