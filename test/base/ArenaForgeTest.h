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
//  ArenaForgeTest.h
//  ArenaForge
//
//  Created by king on 2025/9/4.
//

#ifndef ArenaForgeTest_h_ArenaForge
#define ArenaForgeTest_h_ArenaForge

#include <gtest/gtest.h>

namespace arenaforge {
class ArenaForgeTest : public testing::Test {
  public:
    static bool HasFailure();

    void SetUp() override;

    void TearDown() override;
};

#define ArenaForge_TEST(test_case_name, test_name)                     \
    GTEST_TEST_(test_case_name, test_name, arenaforge::ArenaForgeTest, \
                ::testing::internal::GetTypeId<arenaforge::ArenaForgeTest>())
};  // namespace arenaforge

#endif /* ArenaForgeTest_h_ArenaForge */
