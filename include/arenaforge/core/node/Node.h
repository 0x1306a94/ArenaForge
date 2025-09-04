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
//  Node.h
//  ArenaForge
//
//  Created by king on 2025/9/4.
//

#ifndef Node_h_ArenaForge
#define Node_h_ArenaForge

#include <memory>
#include <vector>

namespace arenaforge {
class Node : std::enable_shared_from_this<Node> {
  public:
    /**
     * Creates a new Node instance.
     */
    static std::shared_ptr<Node> Make();

    virtual ~Node();

    /**
     * Returns the parent node that contains the calling node.
     */
    Node *parent() const {
        return _parent;
    }

    /**
     * Returns the list of child nodes that are direct children of the calling node. Note: Do not
     * iterate through this list directly with a loop while modifying it, as the loop may skip
     * children. Instead, make a copy of the list and iterate through the copy.
     */
    const std::vector<std::shared_ptr<Node>> &children() const {
        return _children;
    }

  protected:
    Node();

  private:
    Node *_parent{nullptr};
    std::vector<std::shared_ptr<Node>> _children = {};
};
};  // namespace arenaforge

#endif /* Node_h_ArenaForge */
