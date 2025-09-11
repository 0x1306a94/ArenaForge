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
//  Venue.h
//  ArenaForge
//
//  Created by king on 2025/9/5.
//

#ifndef Venue_h_ArenaForge
#define Venue_h_ArenaForge

#include <memory>
#include <string>
#include <unordered_map>

#include <tgfx/core/Color.h>
#include <tgfx/core/Rect.h>

namespace tgfx {
class TextLayer;
};

namespace arenaforge {
class Project;
class BaseLayer;
class Venue : public std::enable_shared_from_this<Venue> {
  public:
    static std::shared_ptr<Venue> Make(const std::string &venueId, const std::string &name, const std::string &description);

    static std::shared_ptr<Venue> MakeFromJSONFile(const std::string &jsonFile);

    ~Venue();

    const std::string name() const;
    void setName(const std::string &name);

    const std::string description() const {
        return _description;
    }

    void setDescription(const std::string &description) {
        _description = description;
    }

    std::string venueId() const {
        return _venueId;
    }

    tgfx::Rect frame() const {
        return _frame;
    }

    void setFrame(const tgfx::Rect &frame);

    tgfx::Color backgroundColor() const {
        return _backgroundColor;
    }

    void setBackgroundColor(const tgfx::Color &color);

    std::shared_ptr<Project> project() const;

    std::shared_ptr<BaseLayer> newLayer(const std::string &name);

    std::shared_ptr<BaseLayer> findLayer(const std::string &layerId);

    const BaseLayer *rootLayer() const;
    std::shared_ptr<BaseLayer> rootLayerPtr() const;

    BaseLayer *containerLayer() const;
    std::shared_ptr<BaseLayer> containerLayerPtr() const;

    BaseLayer *maskLayer() const;

    std::string toJSON(bool pretty = false) const;

  protected:
    Venue(const std::string &venueId, const std::string &name, const std::string &description);

  private:
    void attachProject(std::weak_ptr<Project> project);
    void detachProject();

  private:
    std::string _venueId{""};
    std::string _description{""};
    tgfx::Rect _frame{};
    tgfx::Color _backgroundColor = tgfx::Color::Transparent();
    std::shared_ptr<BaseLayer> _rootLayer{nullptr};
    std::shared_ptr<tgfx::TextLayer> _nameLayer{nullptr};
    std::shared_ptr<BaseLayer> _containerLayer{nullptr};
    std::shared_ptr<BaseLayer> _maskLayer{nullptr};
    std::weak_ptr<Project> _ownerProject;
    std::unordered_map<std::string, std::weak_ptr<BaseLayer>> _layerMap{};

    friend class Project;
};
};  // namespace arenaforge

#endif /* Venue_h_ArenaForge */
