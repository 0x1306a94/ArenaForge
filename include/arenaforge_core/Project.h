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
#include <arenaforge_core/Size.h>

namespace arenaforge {
class Layer;
class Project : public std::enable_shared_from_this<Project> {
  public:
    static constexpr float DefaultCanvasWidth = 1400.0f;
    static constexpr float DefaultCanvasHeight = 1200.0f;

    static std::shared_ptr<Project> Make(const std::string &name, const std::string &description, const Size &canvasSize = {DefaultCanvasWidth, DefaultCanvasHeight});

    static std::shared_ptr<Project> MakeFromJSONFile(const std::string &projectDir);

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

    const Size canvasSize() const;

    void setCanvasSize(const Size &canvasSize);

    const std::shared_ptr<Layer> root() const {
        return _root;
    }

    uint32_t genRectangleCounter() {
        return _rectangleCounter++;
    }

    uint32_t genGroupCounter() {
        return _groupCounter++;
    }

    std::string toJSON(bool pretty = false) const;

  protected:
    Project(const std::string &name, const std::string &description, const Size &canvasSize);

  private:
  private:
    std::string _name{""};
    std::string _description{""};
    ProjectVersion _version{ProjectVersion::Version1};
    uint32_t _rectangleCounter{0};
    uint32_t _groupCounter{0};
    Size _canvasSize;
    std::shared_ptr<Layer> _root{};
};
};  // namespace arenaforge

#endif /* Project_h_ArenaForge */
