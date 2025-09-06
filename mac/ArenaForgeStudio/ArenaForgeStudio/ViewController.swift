//
//  ViewController.swift
//  ArenaForgeStudio
//
//  Created by KK on 2025/9/6.
//

import Cocoa

import arenaforge

class ViewController: NSViewController {
    
    var editor: AFEditor?
    var canvasView: AFMacCanvasView!
    
    override func viewDidLoad() {
        super.viewDidLoad()

        // Do any additional setup after loading the view.
        
        editor = AFEditor()
        
        canvasView = AFMacCanvasView()
        canvasView.translatesAutoresizingMaskIntoConstraints = false
        canvasView.wantsLayer = true
        canvasView.layer?.backgroundColor = .white
        
        self.view.addSubview(canvasView)
        NSLayoutConstraint.activate([
            canvasView.leadingAnchor.constraint(equalTo: self.view.leadingAnchor),
            canvasView.topAnchor.constraint(equalTo: self.view.topAnchor),
            canvasView.trailingAnchor.constraint(equalTo: self.view.trailingAnchor),
            canvasView.bottomAnchor.constraint(equalTo: self.view.bottomAnchor),
        ])
    }

    override var representedObject: Any? {
        didSet {
            // Update the view, if already loaded.
        }
    }
}
