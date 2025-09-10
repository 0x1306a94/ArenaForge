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
//  URL+Extensions.swift
//  ArenaForgeStudio
//
//  Created by king on 2025/9/10.
//

import Foundation

extension URL {
    /// Compare a URL using its path components.
    /// - Parameter other: The URL to compare to
    /// - Returns: `true` if the URL points to the same path on disk. Regardless of query parameters, trailing
    ///            slashes, etc.
    func componentCompare(_ other: URL) -> Bool {
        return self.pathComponents == other.pathComponents
    }

    /// Determines if another URL is lower in the file system than this URL.
    ///
    /// Examples:
    /// ```
    /// URL(filePath: "/Users/Bob/Desktop").containsSubPath(URL(filePath: "/Users/Bob/Desktop/file.txt")) // true
    /// URL(filePath: "/Users/Bob/Desktop").containsSubPath(URL(filePath: "/Users/Bob/Desktop/")) // false
    /// URL(filePath: "/Users/Bob/Desktop").containsSubPath(URL(filePath: "/Users/Bob/")) // false
    /// URL(filePath: "/Users/Bob/Desktop").containsSubPath(URL(filePath: "/Users/Bob/Desktop/Folder")) // true
    /// ```
    ///
    /// - Parameter other: The URL to compare.
    /// - Returns: True, if the other URL is lower in the file system.
    func containsSubPath(_ other: URL) -> Bool {
        other.absoluteString.starts(with: absoluteString)
            && other.pathComponents.count > pathComponents.count
    }

    /// Compares this url with another, counting the number of shared path components. Stops counting once a
    /// different component is found.
    ///
    /// - Note: URL treats a leading `/` as a component, so `/Users` and `/` will return `1`.
    /// - Parameter other: The URL to compare against.
    /// - Returns: The number of shared components.
    func sharedComponents(_ other: URL) -> Int {
        var count = 0
        for (component, otherComponent) in zip(pathComponents, other.pathComponents) {
            if component == otherComponent {
                count += 1
            } else {
                return count
            }
        }
        return count
    }
}
