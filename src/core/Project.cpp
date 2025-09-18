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

#include <arenaforge_core/Print.h>
#include <arenaforge_core/layers/Layer.h>
#include <arenaforge_core/uuid/UUID.h>

#include "serialize/JSONSerializeHelper.h"
#include "serialize/LayerJSONHelper.h"

#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>

namespace arenaforge {
std::shared_ptr<Project> Project::Make(const std::string &name, const std::string &description, const Size &canvasSize) {
    return std::shared_ptr<Project>(new Project(name, description, canvasSize));
}

std::shared_ptr<Project> Project::MakeFromJSONFile(const std::string &projectDir) {

    namespace fs = std::filesystem;
    namespace as = arenaforge::json;

    fs::path rootPath{projectDir};
    auto projectPath = rootPath.append("project.json");

    if (!fs::exists(projectPath)) {
        throw std::logic_error("The project.json file is missing.");
    }

    std::ifstream ifs(projectPath);
    nlohmann::json j = nlohmann::json::parse(ifs);
    auto name = as::read_value<std::string>(j, "name", "");
    auto description = as::read_value<std::string>(j, "description", "");
    auto canvasWidth = as::read_value<float>(j, "canvasWidth", DefaultCanvasWidth);
    auto canvasHeight = as::read_value<float>(j, "canvasHeight", DefaultCanvasHeight);
    auto project = Make(name, description, {canvasWidth, canvasHeight});

    project->_version = as::read_value<ProjectVersion>(j, "version", ProjectVersion::Version1);
    project->_rectangleCounter = as::read_value<uint32_t>(j, "rectangleCounter", 0);
    project->_groupCounter = as::read_value<uint32_t>(j, "groupCounter", 0);

    if (j.contains("rootLayer")) {
        auto root = j["rootLayer"].get<std::shared_ptr<Layer>>();
        root->setIsRoot(true);
        project->_root = std::move(root);
    }

    return project;
}

Project::Project(const std::string &name, const std::string &description, const Size &canvasSize)
    : _name(name)
    , _description(description)
    , _canvasSize(canvasSize.isEmpty() ? Size{DefaultCanvasWidth, DefaultCanvasHeight} : canvasSize) {
    auto uuid = UUID::Instance();
    _root = Layer::Make(uuid());
    _root->setFrame(Rect::MakeWH(_canvasSize.width, _canvasSize.height));
    _root->setIsRoot(true);
}

Project::~Project() {
    PrintLog("%s", __PRETTY_FUNCTION__);
}

const Size Project::canvasSize() const {
    auto frame = _root->frame();
    return Size{frame.width(), frame.height()};
}

void Project::setCanvasSize(const Size &canvasSize) {
    _root->setFrame(Rect::MakeWH(canvasSize.width, canvasSize.height));
}

std::string Project::toJSON(bool pretty) const {
    nlohmann::json j;
    j["name"] = name();
    j["description"] = description();
    j["version"] = version();
    j["rectangleCounter"] = _rectangleCounter;
    j["groupCounter"] = _groupCounter;
    j["canvasWidth"] = _canvasSize.width;
    j["canvasHeight"] = _canvasSize.height;
    j["rootLayer"] = _root;
    return j.dump(pretty ? 4 : -1);
}

};  // namespace arenaforge
