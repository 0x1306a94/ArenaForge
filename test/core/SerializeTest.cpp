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

#include <arenaforge_core/layers/BooleanLayer.h>
#include <arenaforge_core/layers/Layer.h>
#include <arenaforge_core/layers/ShapeLayer.h>

#include "../src/core/serialize/LayerJSONHelper.h"

namespace arenaforge {
ArenaForge_TEST(SerializeTest, LayerToJSON) {
    auto root = arenaforge::Layer::Make("4ACB71E3-740E-45E7-BFEA-F432C98FDEF3");
    root->setName("测试场馆");
    root->setFrame(arenaforge::Rect::MakeWH(1280, 600));

    auto shape = arenaforge::ShapeLayer::Make("4BCB71E3-740E-45E7-BFEA-F432C98FDEF3");
    shape->setName("A区");
    shape->addAttribute("floorId", "7828479");
    shape->addAttribute("priceId", "237487");
    shape->setFrame(arenaforge::Rect::MakeXYWH(200, 100, 400, 300));
    shape->setFill(arenaforge::Color::Red());
    shape->setStroke(arenaforge::Color::White());
    root->addChild(shape);

    auto boolean = arenaforge::BooleanLayer::Make("5BCB71E3-740E-45E7-BFEA-F442C98FDEF3");
    auto ring = arenaforge::ShapeLayer::Make("4BCB71E3-740E-45E7-BFEA-F442C98FDEF3");
    ring->setStroke(arenaforge::Color::White());
    ring->setStrokeAlign(arenaforge::StrokeAlign::Inside);
    ring->setLineWidth(30.0f);

    auto line = arenaforge::ShapeLayer::Make("4BCB71E3-740E-45E7-BFEA-F442C98FDEE3");
    line->setStroke(arenaforge::Color::Black());
    line->setStrokeAlign(arenaforge::StrokeAlign::Center);
    line->setLineWidth(6.0f);

    boolean->addChild(ring);
    boolean->addChild(line);

    root->addChild(boolean);

    std::string json = root->toJSON(true);
    printf("%s\n", json.c_str());
};

ArenaForge_TEST(SerializeTest, LayerFromJSON) {
    auto content = R"(
{
    "childrens": [
        {
            "attributes": {
                "floorId": "7828479",
                "priceId": "237487"
            },
            "frame": {
                "h": 300.0,
                "w": 400.0,
                "x": 200.0,
                "y": 100.0
            },
            "id": "4BCB71E3-740E-45E7-BFEA-F432C98FDEF3",
            "locked": false,
            "name": "A区",
            "style": {
                "fill": "#FFFF0000",
                "lineCap": 0,
                "lineJoin": 0,
                "lineWidth": 1.0,
                "stroke": "#FFFFFFFF",
                "strokeAlign": 1
            },
            "type": 1,
            "visible": true
        },
        {
            "boolean": 0,
            "childrens": [
                {
                    "frame": {
                        "h": 0.0,
                        "w": 0.0,
                        "x": 0.0,
                        "y": 0.0
                    },
                    "id": "4BCB71E3-740E-45E7-BFEA-F442C98FDEF3",
                    "locked": false,
                    "name": "",
                    "style": {
                        "lineCap": 0,
                        "lineJoin": 0,
                        "lineWidth": 30.0,
                        "stroke": "#FFFFFFFF",
                        "strokeAlign": 1
                    },
                    "type": 1,
                    "visible": true
                },
                {
                    "frame": {
                        "h": 0.0,
                        "w": 0.0,
                        "x": 0.0,
                        "y": 0.0
                    },
                    "id": "4BCB71E3-740E-45E7-BFEA-F442C98FDEE3",
                    "locked": false,
                    "name": "",
                    "style": {
                        "lineCap": 0,
                        "lineJoin": 0,
                        "lineWidth": 6.0,
                        "stroke": "#FF000000",
                        "strokeAlign": 0
                    },
                    "type": 1,
                    "visible": true
                }
            ],
            "frame": {
                "h": 0.0,
                "w": 0.0,
                "x": 0.0,
                "y": 0.0
            },
            "id": "5BCB71E3-740E-45E7-BFEA-F442C98FDEF3",
            "locked": false,
            "name": "",
            "type": 2,
            "visible": true
        }
    ],
    "frame": {
        "h": 600.0,
        "w": 1280.0,
        "x": 0.0,
        "y": 0.0
    },
    "id": "4ACB71E3-740E-45E7-BFEA-F432C98FDEF3",
    "locked": false,
    "name": "测试场馆",
    "type": 0,
    "visible": true
})";

    auto json = nlohmann::json::parse(content);
    auto root = json.get<std::shared_ptr<arenaforge::Layer>>();

    EXPECT_TRUE(root != nullptr);
    EXPECT_EQ(root->name(), "测试场馆");
    EXPECT_EQ(root->frame(), arenaforge::Rect::MakeWH(1280.0f, 600.0f));
    EXPECT_TRUE(root->children().size() == 1);

    auto shape = std::static_pointer_cast<arenaforge::ShapeLayer>(root->getChildById("4BCB71E3-740E-45E7-BFEA-F432C98FDEF3"));
    EXPECT_TRUE(shape != nullptr);
    EXPECT_EQ(shape->parent(), root.get());
    EXPECT_EQ(shape->frame(), arenaforge::Rect::MakeXYWH(200, 100, 400, 300));
    EXPECT_EQ(shape->fill(), arenaforge::Color::Red());
    EXPECT_EQ(shape->stroke(), arenaforge::Color::White());
    EXPECT_EQ(shape->getAttributes("floorId"), "7828479");
    EXPECT_EQ(shape->getAttributes("priceId"), "237487");
    
    auto boolean = std::static_pointer_cast<arenaforge::BooleanLayer>(root->getChildById("5BCB71E3-740E-45E7-BFEA-F442C98FDEF3"));
    EXPECT_TRUE(boolean != nullptr);
    EXPECT_EQ(boolean->parent(), root.get());
};

};  // namespace arenaforge
