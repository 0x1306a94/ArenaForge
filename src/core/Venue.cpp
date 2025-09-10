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

#include <arenaforge_core/Venue.h>

#include <arenaforge_core/Project.h>
#include <arenaforge_core/layers/BaseLayer.h>
#include <arenaforge_core/uuid/UUID.h>

#include "serialize/BaseLayerJSONHelper.h"

#include <tgfx/layers/Layer.h>
#include <tgfx/layers/SolidColor.h>
#include <tgfx/platform/Print.h>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fmt/format.h>
#include <fstream>

namespace arenaforge {

std::shared_ptr<Venue> Venue::Make(const std::string &venueId, const std::string &name, const std::string &description) {
    return std::shared_ptr<Venue>(new Venue(venueId, name, description));
}

std::shared_ptr<Venue> Venue::MakeFromJSONFile(const std::string &jsonFile) {
    using namespace nlohmann;

    namespace fs = std::filesystem;

    fs::path jsonPath{jsonFile};
    if (!fs::exists(jsonPath)) {
        auto filename = jsonPath.filename().string();
        throw std::logic_error(fmt::format("The {} file is missing.", filename));
    }

    std::ifstream ifs(jsonFile);
    json j = json::parse(ifs);
    auto venueId = j["venueId"].get<std::string>();
    auto name = j["name"].get<std::string>();
    auto description = j["description"].get<std::string>();
    auto venue = Make(venueId, name, description);

    auto frame = j["frame"].get<tgfx::Rect>();
    venue->setFrame(frame);

    if (j.contains("backgroundColor")) {
        auto backgroundColor = j["backgroundColor"].get<tgfx::Color>();
        venue->setBackgroundColor(backgroundColor);
    }

    do {
        auto layersIt = j.find("layers");
        if (layersIt == j.end()) {
            break;
        }

        auto layers = layersIt->get<std::vector<std::shared_ptr<arenaforge::BaseLayer>>>();
        if (layers.empty()) {
            break;
        }

        auto container = venue->container();
        for (auto layer : layers) {
            container->addChild(layer);
        }
    } while (0);

    return venue;
}

Venue::Venue(const std::string &venueId, const std::string &name, const std::string &description)
    : _venueId(venueId)
    , _description(description) {

    auto &uuid = UUID::Instance();
    _root = BaseLayer::Make(uuid(), ShapeType::Rectangle);
    _root->setPositionRelative(false);

    _container = BaseLayer::Make(uuid(), ShapeType::Rectangle);
    _container->setPositionRelative(false);
    _container->setName(name);

    _mask = BaseLayer::Make(uuid(), ShapeType::Rectangle);
    _mask->setPositionRelative(false);

    _root->addChild(_container);
    _root->addChild(_mask);

    _container->setMask(_mask);
}

Venue::~Venue() {
    tgfx::PrintLog("%s", __PRETTY_FUNCTION__);
}

const std::string Venue::name() const {
    return _container->name();
}

void Venue::setName(const std::string &name) {
    _container->setName(name);
}

void Venue::setFrame(const tgfx::Rect &frame) {
    if (_frame == frame) {
        return;
    }
    _frame = frame;

    tgfx::Path rootPath;
    rootPath.addRect(frame);

    auto rootMatrix = tgfx::Matrix::MakeTrans(frame.x(), frame.y());

    tgfx::Path containerPath;
    containerPath.addRect(tgfx::Rect::MakeWH(frame.width(), frame.height()));

    tgfx::Path maskPath = containerPath;

    _root->setPath(rootPath);
    _root->setMatrix(rootMatrix);

    _container->setPath(containerPath);
    _mask->setPath(maskPath);
}

void Venue::setBackgroundColor(const tgfx::Color &color) {
    if (_backgroundColor == color) {
        return;
    }
    _backgroundColor = color;
    _container->setFillStyle(tgfx::SolidColor::Make(color));
    _mask->setFillStyle(tgfx::SolidColor::Make(color));
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

const BaseLayer *Venue::root() const {
    return _root.get();
}

std::shared_ptr<BaseLayer> Venue::rootPtr() const {
    return _root;
}

BaseLayer *Venue::container() const {
    return _container.get();
}

std::shared_ptr<BaseLayer> Venue::containerPtr() const {
    return _container;
}

BaseLayer *Venue::mask() const {
    return _mask.get();
}

std::string Venue::toJSON(bool pretty) const {
    using namespace nlohmann;
    json j;
    j["venueId"] = _venueId;
    j["name"] = name();
    j["description"] = description();
    j["frame"] = _frame;

    if (_backgroundColor != tgfx::Color::Transparent()) {
        j["backgroundColor"] = _backgroundColor;
    }

    json jlayers = json::array();
    for (auto child : _container->children()) {
        auto baseLayer = std::static_pointer_cast<BaseLayer>(child);
        jlayers.push_back(baseLayer);
    }

    j["layers"] = jlayers;

    return j.dump(pretty ? 4 : -1);
}

};  // namespace arenaforge
