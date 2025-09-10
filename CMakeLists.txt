cmake_minimum_required(VERSION 3.13)
project(ArenaForge LANGUAGES C CXX)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/utils.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(TAGRET_NAME_PREFIX arenaforge)

option(BUILD_EDITOR "build editor" ON)
option(BUILD_CORE_TESTS "build core tests" ON)

set(ArenaForge_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(ArenaForge_ROOT_INCLUDE_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(ArenaForge_THIRD_PARTY_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR}/third_party)
set(GOOGLE_TEST_ROOT ${ArenaForge_THIRD_PARTY_ROOT_DIR}/tgfx/third_party/googletest)

message(STATUS "BUILD_EDITOR: ${BUILD_EDITOR}")
message(STATUS "BUILD_CORE_TESTS: ${BUILD_CORE_TESTS}")


set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(third_party/tgfx/third_party/json)

set(TGFX_BUILD_SVG ON)
set(TGFX_BUILD_LAYERS ON)
add_subdirectory(third_party/tgfx)

add_subdirectory(third_party/fmt)
add_subdirectory(src/core)

if(BUILD_EDITOR)
    add_subdirectory(src/editor)
endif()

if(BUILD_CORE_TESTS)
    message(STATUS "GOOGLE_TEST_ROOT: ${GOOGLE_TEST_ROOT}")
    add_subdirectory(${GOOGLE_TEST_ROOT})

    add_subdirectory(test/core)
endif()


