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

namespace arenaforge {
class IdGenerator;
class Project;
class BaseLayer;
class Venue : public std::enable_shared_from_this<Venue> {
  public:
    static std::shared_ptr<Venue> Make(uint32_t venueId, const std::string &name, const std::string &description);

    ~Venue();

    const std::string name() const {
        return _name;
    }

    void setName(const std::string &name) {
        _name = name;
    }

    const std::string description() const {
        return _description;
    }

    void setDescription(const std::string &description) {
        _description = description;
    }

    uint32_t venueId() const {
        return _venueId;
    }

    std::shared_ptr<Project> project() const;

    BaseLayer *root() const;

    BaseLayer *container() const;

    BaseLayer *mask() const;

  protected:
    Venue(uint32_t venueId, const std::string &name, const std::string &description);

  private:
    void attachProject(std::weak_ptr<Project> project);
    void detachProject();

  private:
    uint32_t _venueId{0};
    std::string _name{""};
    std::string _description{""};
    std::unique_ptr<IdGenerator> _idGen{nullptr};
    std::shared_ptr<BaseLayer> _root{nullptr};
    std::shared_ptr<BaseLayer> _container{nullptr};
    std::shared_ptr<BaseLayer> _mask{nullptr};
    std::weak_ptr<Project> _ownerProject;

    friend class Project;
};
};  // namespace arenaforge

#endif /* Venue_h_ArenaForge */
