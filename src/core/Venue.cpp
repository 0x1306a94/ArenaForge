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
#include <tgfx/layers/TextLayer.h>
#include <tgfx/platform/Print.h>

#include <nlohmann/json.hpp>

#include <fmt/format.h>

#include <filesystem>
#include <fstream>
#include <stack>

#if __APPLE__
#include <TargetConditionals.h>
#endif

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

        auto container = venue->containerLayer();
        for (auto layer : layers) {
            container->addChild(layer);
        }
    } while (0);

    return venue;
}

Venue::Venue(const std::string &venueId, const std::string &name, const std::string &description)
    : _venueId(venueId)
    , _description(description) {

    _rootLayer = BaseLayer::Make(venueId + "-root", LayerType::Group);
    _rootLayer->setPositionRelative(false);

    _nameLayer = tgfx::TextLayer::Make();
    _nameLayer->setTextColor(tgfx::Color::White());
    _nameLayer->setText(name);
    _nameLayer->setTextAlign(tgfx::TextAlign::Left);

#if TARGET_OS_MAC
    do {
        auto typeface = tgfx::Typeface::MakeFromName("PingFang SC", "");
        if (!typeface) {
            break;
        }
        auto font = tgfx::Font(std::move(typeface), 16);
        const auto fontMetrics = font.getMetrics();
        auto lineHeight = std::fabs(fontMetrics.ascent) + std::fabs(fontMetrics.descent) + std::fabs(fontMetrics.leading);
        _nameLayer->setFont(font);
        _nameLayer->setPosition(tgfx::Point{0, -(lineHeight + 10)});
    } while (0);
#endif

    _containerLayer = BaseLayer::Make(venueId + "-container", LayerType::Rectangle);
    _containerLayer->setPositionRelative(false);
    _containerLayer->setName(name);

    _maskLayer = BaseLayer::Make(venueId + "-mask", LayerType::Rectangle);
    _maskLayer->setPositionRelative(false);

    _rootLayer->addChild(_containerLayer);
    _rootLayer->addChild(_maskLayer);
    _rootLayer->addChild(_nameLayer);

    _containerLayer->setMask(_maskLayer);
}

Venue::~Venue() {
    tgfx::PrintLog("%s", __PRETTY_FUNCTION__);
}

const std::string Venue::name() const {
    return _containerLayer->name();
}

void Venue::setName(const std::string &name) {
    _containerLayer->setName(name);
    _nameLayer->setText(name);
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

    _rootLayer->setPath(rootPath);
    _rootLayer->setMatrix(rootMatrix);

    _containerLayer->setPath(containerPath);
    _maskLayer->setPath(maskPath);
}

void Venue::setBackgroundColor(const tgfx::Color &color) {
    if (_backgroundColor == color) {
        return;
    }
    _backgroundColor = color;
    _containerLayer->setFillStyle(tgfx::SolidColor::Make(color));
    _maskLayer->setFillStyle(tgfx::SolidColor::Make(color));
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

std::shared_ptr<BaseLayer> Venue::newLayer(const std::string &name) {
    auto &uuid = UUID::Instance();
    auto layer = BaseLayer::Make(uuid(), LayerType::Rectangle);
    layer->setName(name);
    _containerLayer->addChild(layer);
    _layerMap[layer->layerId()] = layer;
    return layer;
}

std::shared_ptr<BaseLayer> Venue::findLayer(const std::string &layerId) {
    // 先查缓存
    auto it = _layerMap.find(layerId);
    if (it != _layerMap.end()) {
        if (it->second.expired()) {
            _layerMap.erase(it);
            return nullptr;
        }
        return it->second.lock();
    }

    if (!_containerLayer) {
        return nullptr;
    }

    std::stack<std::shared_ptr<BaseLayer>> stack;
    stack.push(_containerLayer);

    while (!stack.empty()) {
        auto current = stack.top();
        stack.pop();

        if (current->layerId() == layerId) {
            _layerMap[layerId] = current;
            return current;
        }

        for (const auto &child : current->children()) {
            stack.push(std::static_pointer_cast<BaseLayer>(child));
        }
    }

    return nullptr;
}

const BaseLayer *Venue::rootLayer() const {
    return _rootLayer.get();
}

std::shared_ptr<BaseLayer> Venue::rootLayerPtr() const {
    return _rootLayer;
}

BaseLayer *Venue::containerLayer() const {
    return _containerLayer.get();
}

std::shared_ptr<BaseLayer> Venue::containerLayerPtr() const {
    return _containerLayer;
}

BaseLayer *Venue::maskLayer() const {
    return _maskLayer.get();
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
    for (auto child : _containerLayer->children()) {
        auto baseLayer = std::static_pointer_cast<BaseLayer>(child);
        jlayers.push_back(baseLayer);
    }

    j["layers"] = jlayers;

    return j.dump(pretty ? 4 : -1);
}

};  // namespace arenaforge
