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
//  Project.cpp
//  ArenaForge
//
//  Created by king on 2025/9/4.
//

#include <arenaforge_core/Project.h>

#include <arenaforge_core/Venue.h>
#include <arenaforge_core/uuid/UUID.h>

#include <tgfx/platform/Print.h>

namespace arenaforge {
std::shared_ptr<Project> Project::Make(const std::string &name, const std::string &description) {
    return std::shared_ptr<Project>(new Project(name, description));
}

Project::Project(const std::string &name, const std::string &description)
    : _name(name)
    , _description(description) {
}

Project::~Project() {
    tgfx::PrintLog("%s", __PRETTY_FUNCTION__);
}

std::shared_ptr<Venue> Project::createVenue(const std::string &name, const std::string &description) {
    auto venueId = UUID::Instance().generate();
    auto venue = Venue::Make(venueId, name, description);
    return venue;
}

bool Project::addVenue(std::shared_ptr<Venue> venue) {
    if (!venue) {
        return false;
    }
    auto project = venue->project();
    if (project && project.get() == this) {
        return true;
    }
    venue->attachProject(weak_from_this());
    if (!doContains(venue.get())) {
        _venues.push_back(venue);
    }
    return true;
}

bool Project::removeVenue(std::shared_ptr<Venue> venue) {
    if (!venue || _venues.empty()) {
        return false;
    }

    auto index = doGetVenueIndex(venue.get());
    auto removeed = removeVenueAt(index);
    return removeed == venue;
}

std::shared_ptr<Venue> Project::removeVenueAt(int index) {
    if (index < 0 || static_cast<size_t>(index) >= _venues.size()) {
        tgfx::PrintLog("The supplied index is out of bounds.");
        return nullptr;
    }

    auto venue = _venues[static_cast<size_t>(index)];
    venue->detachProject();
    _venues.erase(_venues.begin() + index);
    return venue;
}

bool Project::contains(std::shared_ptr<Venue> venue) const {
    if (venue == nullptr) {
        return false;
    }
    return doContains(venue.get());
}

int Project::doGetVenueIndex(const Venue *venue) const {
    int index = 0;
    if (venue == nullptr) {
        return -1;
    }

    for (const auto &item : _venues) {
        if (item.get() == venue) {
            return index;
        }
        index++;
    }
    return -1;
}

bool Project::doContains(const Venue *venue) const {
    if (venue == nullptr) {
        return false;
    }
    for (const auto &item : _venues) {
        if (item.get() == venue) {
            return true;
        }
    }
    return false;
}

};  // namespace arenaforge
