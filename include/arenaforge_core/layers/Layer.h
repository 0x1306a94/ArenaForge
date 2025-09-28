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
//  Layer.h
//  ArenaForge
//
//  Created by king on 2025/9/17.
//

#ifndef Layer_h_ArenaForge
#define Layer_h_ArenaForge

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

#include <arenaforge_core/PathCommand.h>
#include <arenaforge_core/Rect.h>
#include <arenaforge_core/layers/LayerType.h>

namespace arenaforge {
class Layer : public std::enable_shared_from_this<Layer> {
  public:
    enum class StructureChangeType {
        ChildAdded,
        ChildRemoved,
        ChildReordered,
    };

    using PropertyChangedCallback = std::function<void(Layer *layer)>;
    using StructureChangedCallback = std::function<void(Layer *parent, Layer *child, StructureChangeType type, int index)>;
    static std::shared_ptr<Layer> Make(const std::string &layerId);
    virtual ~Layer();

    virtual LayerType type() const {
        return LayerType::Group;
    }

    std::string layerId() const {
        return _layerId;
    }

    void setLayerId(const std::string &layerId);

    std::string name() const {
        return _name;
    }

    void setName(const std::string &name);

    bool visible() const {
        return _visible;
    }

    void setVisible(bool value);

    bool locked() const {
        return _locked;
    }

    void setLocked(bool value);

    bool transient() const {
        return _transient;
    }

    void setTransient(bool value);

    Rect frame() const {
        return _frame;
    }

    bool setFrame(const Rect &frame);

    bool isRoot() const {
        return _isRoot;
    }

    Layer *parent() const {
        return _parent;
    }

    const std::vector<std::shared_ptr<Layer>> &children() const {
        return _children;
    }

    bool addChild(std::shared_ptr<Layer> child);

    bool addChildAt(std::shared_ptr<Layer> child, int index);

    bool contains(std::shared_ptr<Layer> child) const;

    int getChildIndex(std::shared_ptr<Layer> child) const;

    void removeFromParent();

    std::shared_ptr<Layer> removeChildAt(int index);

    void removeChildren(int beginIndex = 0, int endIndex = 0x7fffffff);

    bool setChildIndex(std::shared_ptr<Layer> child, int index);

    bool replaceChild(std::shared_ptr<Layer> oldChild, std::shared_ptr<Layer> newChild);

    std::shared_ptr<Layer> getChildById(const std::string &layerId);

    const std::unordered_map<std::string, std::string> &attributes() const {
        return _attributes;
    }

    void clearAttributes();

    void addAttribute(const std::string &key, const std::string &value);
    void setAttributes(std::unordered_map<std::string, std::string> attributes);
    std::optional<std::string> getAttributes(const std::string &key);

    std::string toJSON(bool pretty = false);

    void setOnPropertyChanged(PropertyChangedCallback cb);
    void setOnStructureChanged(StructureChangedCallback cb);

    void disableNotifyPropertyChanged();
    void enableNotifyPropertyChanged();

    void notifyPropertyChanged();
    void notifyStructureChanged(std::shared_ptr<Layer> child, StructureChangeType type, int index);

  protected:
    Layer(const std::string &layerId);

  private:
    int doGetChildIndex(const Layer *child) const;

    bool doContains(const Layer *child) const;

    void setIsRoot(bool value);

  private:
    std::string _layerId{""};
    std::string _name{""};
    Rect _frame{};
    bool _visible{true};
    bool _transient{false};
    bool _locked{false};
    bool _isRoot{false};
    Layer *_parent{nullptr};
    std::vector<std::shared_ptr<Layer>> _children{};
    std::unordered_map<std::string, std::string> _attributes{};
    PropertyChangedCallback _onPropChanged{nullptr};
    StructureChangedCallback _onStructChanged{nullptr};
    bool _disableNotifyPropertyChanged{false};

    friend class Project;
};
};  // namespace arenaforge

#endif /* Layer_h_ArenaForge */
