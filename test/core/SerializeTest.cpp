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
//  SerializeTest.cpp
//  arenaforge_core_test
//
//  Created by king on 2025/9/10.
//

#include "../base/ArenaForgeTest.h"

#include <arenaforge_core/layers/BaseLayer.h>

#include <tgfx/layers/SolidColor.h>

namespace arenaforge {
ArenaForge_TEST(SerializeTest, LayerToJSON) {
    auto layer = arenaforge::BaseLayer::Make("4ACB71E3-740E-45E7-BFEA-F432C98FDEF3", arenaforge::ShapeType::Rectangle);
    layer->setName("test");
    layer->setFrame(tgfx::Rect::MakeXYWH(200, 200, 400, 400));
    layer->setFillStyle(tgfx::SolidColor::Make(tgfx::Color::Red()));
    layer->setStrokeStyle(tgfx::SolidColor::Make(tgfx::Color::White()));
    layer->setLineWidth(6);

    std::string json = layer->toJSON(true);
    printf("%s\n", json.c_str());
};
};  // namespace arenaforge
