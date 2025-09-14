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
//  LayerNavigatorViewController.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/7.
//

import AppKit
import arenaforge_editor

final class LayerNavigatorViewController: NSViewController {
    weak var project: ProjectDocument?

    private var scrollView: NSScrollView!
    var outlineView: NSOutlineView!

    var venues: [AFVenue] {
        project?.project?.venues ?? []
    }

    var rowHeight: Double = 22 {
        willSet {
            if newValue != rowHeight {
                outlineView.rowHeight = newValue
                outlineView.reloadData()
            }
        }
    }

    var expandedItems: NSSet = []

    init(project: ProjectDocument) {
        super.init(nibName: nil, bundle: nil)
        self.project = project
//        let venues = project.project?.venues ?? []
//        self.venues = venues
    }

    /// Setup the ``scrollView`` and ``outlineView``
    override func loadView() {
        scrollView = NSScrollView()
        scrollView.hasVerticalScroller = true
        view = scrollView

        outlineView = NSOutlineView()
        outlineView.dataSource = self
        outlineView.delegate = self
        outlineView.autosaveExpandedItems = true
        outlineView.autosaveName = project?.fileURL?.path() ?? ""
        outlineView.autosaveExpandedItems = true
        outlineView.headerView = nil
        outlineView.menu = LayerNavigatorMenu(self, document: project)
        outlineView.menu?.delegate = self
        outlineView.doubleAction = #selector(onItemDoubleClicked)
        outlineView.allowsMultipleSelection = true

        outlineView.setAccessibilityIdentifier("LayerNavigator")
        outlineView.setAccessibilityLabel("Layer Navigator")

        let column = NSTableColumn(identifier: .init(rawValue: "Cell"))
        column.title = "Cell"
        outlineView.addTableColumn(column)

        outlineView.registerForDraggedTypes([.string])
        outlineView.setDraggingSourceOperationMask(.move, forLocal: false)

        scrollView.documentView = outlineView
        scrollView.contentView.automaticallyAdjustsContentInsets = false
        scrollView.contentView.contentInsets = .init(top: 10, left: 0, bottom: 0, right: 0)
        scrollView.scrollerStyle = .overlay
        scrollView.hasVerticalScroller = true
        scrollView.hasHorizontalScroller = false
        scrollView.autohidesScrollers = true

//        outlineView.expandItem(outlineView.item(atRow: 0))

        /// Get autosave expanded items.
//        for row in 0 ..< outlineView.numberOfRows {
//            if let item = outlineView.item(atRow: row) as? AnyObject {
//                if outlineView.isItemExpanded(item) {
//                    expandedItems.adding(item)
//                }
//            }
//        }
    }

    init() {
        super.init(nibName: nil, bundle: nil)
    }

    @available(*, unavailable)
    required init?(coder: NSCoder) {
        fatalError()
    }

    @objc
    func onItemDoubleClicked() {}

    func onNewVeuen(_ venue: AFVenue) {
        project?.undoManager?.registerUndo(withTarget: self) {
            $0.project?.project?.removeVenue(venue)
            $0.outlineView.reloadData()
        }

        outlineView.reloadData()
    }

    func onVeuenAddShape(_ venue: AFVenue, shape: AFLayer) {
        let index = outlineView.row(forItem: venue.root)
        if index == -1 {
            return
        }

        project?.undoManager?.registerUndo(withTarget: self) {
            shape.removeFromParent()
            $0.outlineView.reloadItem(venue.root, reloadChildren: true)
        }

        outlineView.reloadItem(venue.root, reloadChildren: true)
    }

    #if DEBUG
        deinit {
            print("\(type(of: self)) deinit")
        }
    #endif
}
