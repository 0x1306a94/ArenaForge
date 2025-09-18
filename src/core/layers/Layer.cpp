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
//  Layer.cpp
//  ArenaForge
//
//  Created by king on 2025/9/17.
//

#include <arenaforge_core/layers/Layer.h>

#include "serialize/LayerJSONHelper.h"

namespace arenaforge {
std::shared_ptr<Layer> Layer::Make(const std::string &layerId) {
    return std::shared_ptr<Layer>(new Layer(layerId));
}

Layer::Layer(const std::string &layerId)
    : _layerId(layerId) {
}

Layer::~Layer() {
}

void Layer::setLayerId(const std::string &layerId) {
    _layerId = layerId;
}

void Layer::setName(const std::string &name) {
    _name = name;
}

void Layer::setVisible(bool value) {
    if (_visible == value) {
        return;
    }
    _visible = value;
    notifyPropertyChanged();
}

void Layer::setLocked(bool value) {
    if (_locked == value) {
        return;
    }
    _locked = value;
}

void Layer::setTransient(bool value) {
    if (_transient == value) {
        return;
    }
    _transient = value;
}

void Layer::setFrame(const Rect &frame) {
    if (_frame == frame) {
        return;
    }
    _frame = frame;
    notifyPropertyChanged();
}

bool Layer::addChild(std::shared_ptr<Layer> child) {
    if (!child) {
        return false;
    }
    auto index = _children.size();
    if (child->_parent == this) {
        index--;
    }
    return addChildAt(child, static_cast<int>(index));
}

bool Layer::addChildAt(std::shared_ptr<Layer> child, int index) {
    if (!child) {
        return false;
    }
    if (child.get() == this) {
        return false;
    } else if (child->doContains(this)) {
        return false;
    }
    if (child->_parent == this) {
        return setChildIndex(child, index);
    }
    child->removeFromParent();
    _children.insert(_children.begin() + index, child);
    child->_parent = this;
    notifyStructureChanged(child, StructureChangeType::ChildAdded, index);
    return true;
}

bool Layer::contains(std::shared_ptr<Layer> child) const {
    return doContains(child.get());
}

int Layer::getChildIndex(std::shared_ptr<Layer> child) const {
    return doGetChildIndex(child.get());
}

void Layer::removeFromParent() {
    if (!_parent) {
        return;
    }
    _parent->removeChildAt(_parent->doGetChildIndex(this));
}

std::shared_ptr<Layer> Layer::removeChildAt(int index) {
    if (index < 0 || static_cast<size_t>(index) >= _children.size()) {
        return nullptr;
    }
    auto child = _children[static_cast<size_t>(index)];
    child->_parent = nullptr;
    _children.erase(_children.begin() + index);
    notifyStructureChanged(child, StructureChangeType::ChildRemoved, index);
    return child;
}

void Layer::removeChildren(int beginIndex, int endIndex) {
    if (_children.empty()) {
        return;
    }
    if (beginIndex < 0 || static_cast<size_t>(beginIndex) >= _children.size()) {
        return;
    }
    if (endIndex < 0 || static_cast<size_t>(endIndex) >= _children.size()) {
        endIndex = static_cast<int>(_children.size()) - 1;
    }
    for (int i = endIndex; i >= beginIndex; --i) {
        removeChildAt(i);
    }
}

bool Layer::setChildIndex(std::shared_ptr<Layer> child, int index) {
    if (index < 0 || static_cast<size_t>(index) >= _children.size()) {
        index = static_cast<int>(_children.size()) - 1;
    }
    auto oldIndex = getChildIndex(child);
    if (oldIndex < 0) {
        return false;
    }
    if (oldIndex == index) {
        return true;
    }
    _children.erase(_children.begin() + oldIndex);
    _children.insert(_children.begin() + index, child);
    notifyStructureChanged(child, StructureChangeType::ChildReordered, index);
    return true;
}

bool Layer::replaceChild(std::shared_ptr<Layer> oldChild, std::shared_ptr<Layer> newChild) {
    auto index = getChildIndex(oldChild);
    if (index < 0) {
        return false;
    }
    if (!addChildAt(newChild, index)) {
        return false;
    }
    oldChild->removeFromParent();
    return true;
}

std::shared_ptr<Layer> Layer::getChildById(const std::string &layerId) {
    for (auto &child : _children) {
        if (child->layerId() == layerId) {
            return child;
        }
    }
    return nullptr;
}

void Layer::clearAttributes() {
    _attributes.clear();
}

void Layer::addAttribute(const std::string &key, const std::string &value) {
    _attributes[key] = value;
}

void Layer::setAttributes(std::unordered_map<std::string, std::string> attributes) {
    _attributes = std::move(attributes);
}

std::optional<std::string> Layer::getAttributes(const std::string &key) {
    auto it = _attributes.find(key);
    if (it == _attributes.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::string Layer::toJSON(bool pretty) {
    nlohmann::json j = shared_from_this();
    return j.dump(pretty ? 4 : -1);
}

void Layer::setOnPropertyChanged(PropertyChangedCallback cb) {
    _onPropChanged = std::move(cb);
}

void Layer::setOnStructureChanged(StructureChangedCallback cb) {
    _onStructChanged = std::move(cb);
}

void Layer::notifyPropertyChanged() {
    if (_onPropChanged) {
        _onPropChanged(this);
    }
}

void Layer::notifyStructureChanged(std::shared_ptr<Layer> child, StructureChangeType type, int index) {
    if (_onStructChanged && child) {
        _onStructChanged(this, child.get(), type, index);
    }
}

int Layer::doGetChildIndex(const Layer *child) const {
    int index = 0;
    for (auto &layer : _children) {
        if (layer.get() == child) {
            return index;
        }
        index++;
    }
    return -1;
}

bool Layer::doContains(const Layer *child) const {
    auto target = child;
    while (target) {
        if (target == this) {
            return true;
        }
        target = target->_parent;
    }
    return false;
}

void Layer::setIsRoot(bool value) {
    _isRoot = value;
}

};  // namespace arenaforge
