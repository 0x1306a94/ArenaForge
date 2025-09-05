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
//  Venue.cpp
//  ArenaForge
//
//  Created by king on 2025/9/5.
//

#include <arenaforge/core/Venue.h>

#include <arenaforge/core/IdGenerator.h>
#include <arenaforge/core/Project.h>
#include <arenaforge/core/layers/BaseLayer.h>

#include <tgfx/layers/Layer.h>
#include <tgfx/platform/Print.h>

namespace arenaforge {

std::shared_ptr<Venue> Venue::Make(uint32_t venueId, const std::string &name, const std::string &description) {
    return std::shared_ptr<Venue>(new Venue(venueId, name, description));
}

Venue::Venue(uint32_t venueId, const std::string &name, const std::string &description)
    : _venueId(venueId)
    , _name(name)
    , _description(description)
    , _idGen(std::make_unique<IdGenerator>(1)) {

    _root = BaseLayer::Make(_idGen->generate());
    _container = BaseLayer::Make(_idGen->generate());
    _mask = BaseLayer::Make(_idGen->generate());

    _root->addChild(_container);
    _root->addChild(_mask);

    _container->setMask(_mask);
}

Venue::~Venue() {
    tgfx::PrintLog("%s", __PRETTY_FUNCTION__);
}

void Venue::attachProject(std::weak_ptr<Project> project) {
    _ownerProject = std::move(project);
}

void Venue::detachProject() {
    _ownerProject.reset();
}

std::shared_ptr<Project> Venue::project() const {
    if (_ownerProject.expired()) {
        return nullptr;
    }
    return _ownerProject.lock();
}

BaseLayer *Venue::root() const {
    return _root.get();
}

BaseLayer *Venue::container() const {
    return _container.get();
}

BaseLayer *Venue::mask() const {
    return _mask.get();
}

};  // namespace arenaforge
