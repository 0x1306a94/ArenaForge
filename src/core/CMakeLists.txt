cmake_minimum_required(VERSION 3.13)

set(TARGET_NAME_SUFFIX core)
set(TAGRGET_NAME ${TAGRET_NAME_PREFIX}_${TARGET_NAME_SUFFIX})

message(STATUS "========== Configuration ${TAGRGET_NAME} ==========")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_definitions(
        -Werror -Wall -Wextra -Weffc++ -Wconversion -pedantic -Werror=return-type
    )
endif ()

if (MSVC)
    add_compile_options("/utf-8")
endif (MSVC)




if(APPLE)
    find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
    list(APPEND CORE_SHARED_LIBS ${CORE_FOUNDATION})
endif()



add_files_by_extension(SRC_FILES ".h;.hpp;.cpp"
    .
)

add_files_by_extension(PUBLIC_HEADER_FILES ".h;.hpp"
    ${ArenaForge_ROOT_INCLUDE_DIR}/${TAGRGET_NAME}
    ${ArenaForge_ROOT_INCLUDE_DIR}/${TAGRGET_NAME}
)

message(STATUS "PUBLIC_HEADER_FILES: ${PUBLIC_HEADER_FILES}")
message(STATUS "SRC_FILES: ${SRC_FILES}")
message(STATUS "CORE_SHARED_LIBS: ${CORE_SHARED_LIBS}")

add_library(${TAGRGET_NAME} STATIC ${SRC_FILES} ${PUBLIC_HEADER_FILES})
add_library(${TAGRET_NAME_PREFIX}::${TARGET_NAME_SUFFIX} ALIAS ${TAGRGET_NAME})

add_source_group(PUBLIC_HEADER_FILES ${ArenaForge_ROOT_INCLUDE_DIR}/${TAGRGET_NAME} "PublicHeaders")
add_source_group(SRC_FILES ${CMAKE_CURRENT_LIST_DIR} "Sources")

target_link_libraries(${TAGRGET_NAME}
    ${CORE_SHARED_LIBS}
    tgfx
)

target_include_directories(${TAGRGET_NAME} PRIVATE
    .
    ${ArenaForge_ROOT_INCLUDE_DIR}
)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    target_compile_definitions(${TAGRGET_NAME} PRIVATE DEBUG)
endif ()