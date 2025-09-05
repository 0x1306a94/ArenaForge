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
//  ProjectTest.cpp
//  arenaforge_core_test
//
//  Created by king on 2025/9/5.
//

#include "../base/ArenaForgeTest.h"

#include <arenaforge/core/Project.h>
#include <arenaforge/core/Venue.h>

namespace arenaforge {
ArenaForge_TEST(ProjectTest, Create) {
    auto project = Project::Make("test", "test");
    EXPECT_EQ(project->name(), "test");
    EXPECT_EQ(project->description(), "test");

    auto venue = project->createVenue("test", "test");
    EXPECT_EQ(venue->name(), "test");
    EXPECT_EQ(venue->description(), "test");
    EXPECT_TRUE(venue->project() == nullptr);

    EXPECT_TRUE(project->addVenue(venue));
    EXPECT_EQ(venue->project(), project);

    EXPECT_TRUE(project->contains(venue));
    
    EXPECT_TRUE(project->removeVenue(venue));
    
    EXPECT_FALSE(project->contains(venue));
};
};  // namespace arenaforge
