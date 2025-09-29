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

import arenaforge_editor
import SwiftUI

struct InspectorFrameView: View {
    @EnvironmentObject private var document: ProjectDocument
    @EnvironmentObject private var editor: EditorViewModel

    @State private var x: Double = 0
    @State private var y: Double = 0
    @State private var width: Double = 0
    @State private var height: Double = 0
    @State private var isRoot: Bool = false
    @State private var isLine: Bool = false

    @State private var backupLineFrameWidth: Double?
    @State private var backupPosition: CGPoint?
    @State private var backupSize: CGSize?

    var body: some View {
        VStack(alignment: .leading, spacing: 4) {
            Text("Frame")
                .font(.subheadline)
                .foregroundStyle(.iconSecondary)

            HStack(spacing: 6) {
                DecimalInputField(label: "X", initialValue: x) { newValue, reason in
                    print("X: \(newValue) \(reason)")
                    updateLayerPosition(value: CGPoint(x: newValue, y: y), reason: reason)
                }
                .disabled(isRoot)

                DecimalInputField(label: "Y", initialValue: y) { newValue, reason in
                    print("Y: \(newValue) \(reason)")
                    updateLayerPosition(value: CGPoint(x: x, y: newValue), reason: reason)
                }
                .disabled(isRoot)
            }

            HStack(spacing: 6) {
                DecimalInputField(label: "W", initialValue: width) { newValue, reason in
                    print("W: \(newValue) \(reason)")
                    if isLine {
                        updateLineLayerWidth(value: newValue, reason: reason)
                    } else if isRoot {
                        updateVenueSize(value: CGSize(width: newValue, height: height), reason: reason)
                    } else {
                        updateLayerSize(value: CGSize(width: newValue, height: height), reason: reason)
                    }
                }

                DecimalInputField(label: "H", initialValue: height) { newValue, reason in
                    print("H: \(newValue) \(reason)")
                    if isRoot {
                        updateVenueSize(value: CGSize(width: width, height: newValue), reason: reason)
                    } else {
                        updateLayerSize(value: CGSize(width: width, height: newValue), reason: reason)
                    }
                }
                .disabled(isLine)
            }
        }
        .padding(.horizontal, 10)
        .frame(maxWidth: .infinity)
        .onAppear {
            updateInspectorSource()
        }
        .onChange(of: document.activeLayer) { _, _ in
            updateInspectorSource()
        }
    }

    private func updateInspectorSource() {
        isRoot = document.activeLayer?.isRoot ?? false
        let frame = document.activeLayer?.frame() ?? .zero
        x = frame.minX
        y = frame.minY

        if let lineLayer = document.activeLayer as? AFLineLayer {
            isLine = true
            width = lineLayer.width
            height = 0
        } else {
            isLine = false
            width = frame.width
            height = frame.height
        }
    }

    private func updateLayerPosition(value: CGPoint, reason: DecimalInputField.ValueChangeReason) {
        guard let layer = document.activeLayer else { return }

        var newFrame = layer.frame()
        newFrame.origin = value

        switch reason {
        case .enterKey, .focusLost:
            let oldPosition = backupPosition ?? layer.frame().origin
            var oldFrame = layer.frame()
            oldFrame.origin = oldPosition

            defer { backupPosition = nil }
            guard layer.updateFrame(newFrame) else {
                return
            }
            updateInspectorSource()

            document.undoManager?.registerUndo(withTarget: layer, handler: {
                $0.updateFrame(oldFrame)
                updateInspectorSource()
            })
        case .sliderBegin:
            backupPosition = layer.frame().origin
        case .slider:
            layer.updateFrame(newFrame)
        case .sliderEnd:
            let oldPosition = backupPosition ?? layer.frame().origin
            var oldFrame = layer.frame()
            oldFrame.origin = oldPosition

            defer { backupPosition = nil }
            guard layer.updateFrame(newFrame) else {
                return
            }
            updateInspectorSource()

            document.undoManager?.registerUndo(withTarget: layer, handler: {
                $0.updateFrame(oldFrame)
                updateInspectorSource()
            })
        }
    }

    private func updateLineLayerWidth(value: Double, reason: DecimalInputField.ValueChangeReason) {
        if value <= 0 {
            width = 0
            return
        }

        if let lineLayer = document.activeLayer as? AFLineLayer {
            switch reason {
            case .enterKey, .focusLost:
                let oldWidth = backupLineFrameWidth ?? lineLayer.width

                defer { backupLineFrameWidth = nil }
                guard oldWidth != value else {
                    return
                }
                lineLayer.width = value
                updateInspectorSource()

                document.undoManager?.registerUndo(withTarget: lineLayer, handler: {
                    $0.width = oldWidth
                    updateInspectorSource()
                })
            case .sliderBegin:
                backupLineFrameWidth = lineLayer.width
            case .slider:
                lineLayer.width = value
            case .sliderEnd:
                let oldWidth = backupLineFrameWidth ?? lineLayer.width
                defer { backupLineFrameWidth = nil }
                guard oldWidth != value else {
                    return
                }
                lineLayer.width = value
                updateInspectorSource()

                document.undoManager?.registerUndo(withTarget: lineLayer, handler: {
                    $0.width = oldWidth
                    updateInspectorSource()
                })
            }
        }
    }

    private func updateLayerSize(value: CGSize, reason: DecimalInputField.ValueChangeReason) {
        guard !isLine else {
            return
        }

        guard let layer = document.activeLayer else { return }

        var newFrame = layer.frame()
        newFrame.size = value

        switch reason {
        case .enterKey, .focusLost:
            let oldSize = backupSize ?? layer.frame().size
            var oldFrame = layer.frame()
            oldFrame.size = oldSize
            defer { backupSize = nil }

            guard layer.updateFrame(newFrame) else {
                return
            }
            updateInspectorSource()

            document.undoManager?.registerUndo(withTarget: layer, handler: {
                $0.updateFrame(oldFrame)
                updateInspectorSource()
            })
        case .sliderBegin:
            backupSize = layer.frame().size
        case .slider:
            layer.updateFrame(newFrame)
        case .sliderEnd:
            let oldSize = backupSize ?? layer.frame().size
            var oldFrame = layer.frame()
            oldFrame.size = oldSize

            defer { backupSize = nil }
            guard layer.updateFrame(newFrame) else {
                return
            }

            updateInspectorSource()

            document.undoManager?.registerUndo(withTarget: layer, handler: {
                $0.updateFrame(oldFrame)
                updateInspectorSource()
            })
        }
    }

    private func updateVenueSize(value: CGSize, reason: DecimalInputField.ValueChangeReason) {
        guard isRoot else {
            return
        }

        guard let project = document.project else {
            return
        }

        switch reason {
        case .enterKey, .focusLost:
            let oldSize = project.canvasSize()
            defer { backupSize = nil }
            guard project.updateCanvasSize(value) else {
                return
            }
            updateInspectorSource()

            document.undoManager?.registerUndo(withTarget: project, handler: {
                $0.updateCanvasSize(oldSize)
                updateInspectorSource()
            })
        case .sliderBegin:
            backupSize = project.canvasSize()
        case .slider:
            project.updateCanvasSize(value)
        case .sliderEnd:
            let oldSize = backupSize ?? project.canvasSize()
            defer { backupSize = nil }

            guard project.updateCanvasSize(value) else {
                return
            }

            updateInspectorSource()

            document.undoManager?.registerUndo(withTarget: project, handler: {
                $0.updateCanvasSize(oldSize)
                updateInspectorSource()
            })
        }
    }
}

#Preview {
    InspectorFrameView()
}
