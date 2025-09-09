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

#include <tgfx/layers/Layer.h>
#include <tgfx/layers/SolidColor.h>
#include <tgfx/platform/Print.h>

#include <nlohmann/json.hpp>

#include <fstream>
namespace arenaforge {

std::shared_ptr<Venue> Venue::Make(const std::string &venueId, const std::string &name, const std::string &description) {
    return std::shared_ptr<Venue>(new Venue(venueId, name, description));
}

std::shared_ptr<Venue> Venue::MakeFromJSONFile(const std::string &jsonFile) {
    using namespace nlohmann;

    std::ifstream ifs(jsonFile);
    json j = json::parse(ifs);
    auto venueId = j["venueId"].get<std::string>();
    auto name = j["name"].get<std::string>();
    auto description = j["description"].get<std::string>();
    auto venue = Make(venueId, name, description);

    auto jframe = j["frame"];
    auto x = jframe["x"].get<float>();
    auto y = jframe["y"].get<float>();
    auto width = jframe["width"].get<float>();
    auto height = jframe["height"].get<float>();
    venue->setFrame(tgfx::Rect::MakeXYWH(x, y, width, height));

    if (j.contains("backgroundColor")) {
        auto backgroundColor = j["backgroundColor"];
        auto r = backgroundColor["r"].get<uint8_t>();
        auto g = backgroundColor["g"].get<uint8_t>();
        auto b = backgroundColor["b"].get<uint8_t>();
        auto a = backgroundColor["a"].get<uint8_t>();
        venue->setBackgroundColor(tgfx::Color::FromRGBA(r, g, b, a));
    }

    return venue;
}

Venue::Venue(const std::string &venueId, const std::string &name, const std::string &description)
    : _venueId(venueId)
    , _name(name)
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

    _root->setPath(rootPath);
    _root->setMatrix(rootMatrix);

    _container->setPath(containerPath);
    _mask->setPath(containerPath);
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

std::string Venue::toJSON() const {
    using namespace nlohmann;
    json j;
    j["venueId"] = _venueId;
    j["name"] = name();
    j["description"] = description();

    j["frame"] = json{
        {"x", _frame.x()},
        {"y", _frame.y()},
        {"width", _frame.width()},
        {"height", _frame.height()},
    };

    j["backgroundColor"] = json{
        {"r", static_cast<uint8_t>(_backgroundColor.red * 255.0f)},
        {"g", static_cast<uint8_t>(_backgroundColor.green * 255.0f)},
        {"b", static_cast<uint8_t>(_backgroundColor.blue * 255.0f)},
        {"a", static_cast<uint8_t>(_backgroundColor.alpha * 255.0f)},
    };

    json jlayers = json::array();
    j["layers"] = jlayers;
    return j.dump(4);
}

};  // namespace arenaforge
