cmake_minimum_required(VERSION 3.13)

set(TARGET_NAME_SUFFIX core_test)
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

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif ()


add_files_by_extension(SRC_FILES ".h;.hpp;.cpp"
    .
    ../base
)

message(STATUS "SRC_FILES: ${SRC_FILES}")

add_executable(${TAGRGET_NAME} ${SRC_FILES})
target_link_libraries(${TAGRGET_NAME} 
    ${TAGRET_NAME_PREFIX}_core
    gtest
)

target_include_directories(${TAGRGET_NAME} PRIVATE
    .
    ${ArenaForge_ROOT_INCLUDE_DIR}
    ${GOOGLE_TEST_ROOT}/googletest/include
)

