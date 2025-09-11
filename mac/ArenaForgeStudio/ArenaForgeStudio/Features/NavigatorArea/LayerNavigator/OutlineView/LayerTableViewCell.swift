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
//  LayerTableViewCell.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/9.
//

import AppKit
import arenaforge_editor

class SpecialSelectTextField: NSTextField {
    override func becomeFirstResponder() -> Bool {
        let range = NSRange(
            location: 0,
            length: stringValue.distance(
                from: stringValue.startIndex,
                to: stringValue.lastIndex(of: ".") ?? stringValue.endIndex
            )
        )
        selectText(self)
        let editor = currentEditor()
        editor?.selectedRange = range
        return true
    }

    override func textDidBeginEditing(_ notification: Notification) {
        super.textDidBeginEditing(notification)
        wantsLayer = true
        layer?.backgroundColor = NSColor.textBackgroundColor.cgColor
    }

    override func textDidEndEditing(_ notification: Notification) {
        super.textDidEndEditing(notification)
        wantsLayer = false
        layer?.backgroundColor = nil
    }
}

protocol LayerTableViewCellDelegate: AnyObject {
    func layerTableViewCellDidFinishEditing(_ cell: LayerTableViewCell)
}

final class LayerTableViewCell: NSTableCellView, NSTextFieldDelegate {
    private var fontSize: Double {
        switch self.frame.height {
        case 20: return 11
        case 22: return 13
        case 24: return 14
        default: return 13
        }
    }

    weak var layerItem: AFLayer?
    weak var delegate: LayerTableViewCellDelegate?

    init(frame frameRect: NSRect, item: AFLayer?, isEditable: Bool = true) {
        super.init(frame: frameRect)
        self.layerItem = item
        setupViews(frame: frameRect, isEditable: isEditable)
        updateModel()

        createConstraints(frame: frameRect)
    }

    override init(frame frameRect: NSRect) {
        super.init(frame: frameRect)
        setupViews(frame: frameRect, isEditable: false)
    }

    @available(*, unavailable)
    required init?(coder: NSCoder) {
        fatalError("init(coder:) has not been implemented")
    }

    func setupViews(frame frameRect: NSRect, isEditable: Bool) {
        let label = createLabel()
        configLabel(label: label, isEditable: isEditable)
        label.delegate = self
        self.textField = label

        addSubview(label)
    }

    func createLabel() -> NSTextField {
        return SpecialSelectTextField(frame: .zero)
    }

    func configLabel(label: NSTextField, isEditable: Bool) {
        label.translatesAutoresizingMaskIntoConstraints = false
        label.drawsBackground = false
        label.isBordered = false
        label.isEditable = isEditable
        label.isSelectable = isEditable
        label.layer?.cornerRadius = 10.0
        label.font = .labelFont(ofSize: fontSize)
        label.lineBreakMode = .byTruncatingMiddle
    }

    func createConstraints(frame frameRect: NSRect) {
        resizeSubviews(withOldSize: .zero)
    }

    func updateModel() {
        let layerName = self.layerItem?.name ?? ""
        let fontSize = textField?.font?.pointSize ?? 12

        let paragraphStyle = NSMutableParagraphStyle()
        paragraphStyle.lineBreakMode = .byTruncatingMiddle

        let attributedString = NSMutableAttributedString(string: layerName, attributes: [
            .paragraphStyle: paragraphStyle,
            .font: NSFont.systemFont(ofSize: fontSize),
            .foregroundColor: NSColor.labelColor
        ])

        self.textField?.attributedStringValue = attributedString
    }

    override func resizeSubviews(withOldSize oldSize: NSSize) {
        super.resizeSubviews(withOldSize: oldSize)

        guard let textField else { return }

        let mainLabelWidth = textField.frame.size.width
        let newSize = textField.sizeThatFits(CGSize(width: mainLabelWidth, height: CGFloat.greatestFiniteMagnitude))
        textField.frame = NSRect(
            x: 2,
            y: 2.5,
            width: ceil(newSize.width),
            height: 25
        )
    }

    // MARK: NSTextFieldDelegate

    func controlTextDidChange(_ obj: Notification) {}

    func controlTextDidEndEditing(_ obj: Notification) {
        self.layerItem?.name = self.textField?.stringValue.trimmingCharacters(in: .whitespacesAndNewlines) ?? ""
        self.delegate?.layerTableViewCellDidFinishEditing(self)
    }
}
