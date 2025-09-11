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
//  Project.h
//  ArenaForge
//
//  Created by king on 2025/9/4.
//

#ifndef Project_h_ArenaForge
#define Project_h_ArenaForge

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include <arenaforge_core/ProjectVersion.h>

namespace arenaforge {
class Venue;
class Project : public std::enable_shared_from_this<Project> {
  public:
    static std::shared_ptr<Project> Make(const std::string &name, const std::string &description);

    static std::shared_ptr<Project> MakeFromJSONFile(const std::string &jsonFile, std::function<std::shared_ptr<Venue>(const std::string &venuedId)> venuedCreater);

    std::shared_ptr<Venue> createVenue(const std::string &name, const std::string &description);

    ~Project();

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

    ProjectVersion version() const {
        return _version;
    }

    const std::vector<std::shared_ptr<Venue>> venues() const {
        return _venues;
    }

    uint32_t genVenueCounter() {
        return _venueCounter++;
    }

    uint32_t genRectangleCounter() {
        return _rectangleCounter++;
    }
    
    uint32_t genGroupCounter() {
        return _groupCounter++;
    }

    bool addVenue(std::shared_ptr<Venue> venue);
    bool removeVenue(std::shared_ptr<Venue> venue);
    std::shared_ptr<Venue> removeVenueAt(int index);
    bool contains(std::shared_ptr<Venue> venue) const;

    std::string toJSON(bool pretty = false) const;

  protected:
    Project(const std::string &name, const std::string &description);

  private:
    int doGetVenueIndex(const Venue *venue) const;
    bool doContains(const Venue *venue) const;

  private:
    std::string _name{""};
    std::string _description{""};
    ProjectVersion _version{ProjectVersion::Version1};
    uint32_t _venueCounter{0};
    uint32_t _rectangleCounter{0};
    uint32_t _groupCounter{0};
    std::vector<std::shared_ptr<Venue>> _venues{};
};
};  // namespace arenaforge

#endif /* Project_h_ArenaForge */
