cmake_minimum_required(VERSION 3.13)

set(TARGET_NAME_SUFFIX editor)
set(TAGRGET_NAME ${TAGRET_NAME_PREFIX}_${TARGET_NAME_SUFFIX})

if(APPLE)
    enable_language(OBJC OBJCXX)
    option(BUILD_EDITOR_FRAMEWORK "Enable building editor framework" ON)
else()
    set(BUILD_EDITOR_FRAMEWORK OFF)
endif()

message(STATUS "========== Configuration ${TAGRGET_NAME} ==========")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
cmake_policy(SET CMP0063 NEW)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

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


if(MACOS)
    # find_library(CORE_FOUNDATION CoreFoundation REQUIRED)
    # list(APPEND CORE_SHARED_LIBS ${CORE_FOUNDATION})
endif()

set(PUBLIC_HEADER_ROOT_DIR ${ArenaForge_ROOT_INCLUDE_DIR}/${TAGRGET_NAME})



if(APPLE)
    list(APPEND PUBLIC_HEADER_FILES 
        ${PUBLIC_HEADER_ROOT_DIR}/arenaforge_editor.h
        ${PUBLIC_HEADER_ROOT_DIR}/core/defines.h
        ${PUBLIC_HEADER_ROOT_DIR}/core/Platform.h
        ${PUBLIC_HEADER_ROOT_DIR}/core/AFProject.h
        ${PUBLIC_HEADER_ROOT_DIR}/core/AFVenue.h
        ${PUBLIC_HEADER_ROOT_DIR}/core/AFLayer.h
        ${PUBLIC_HEADER_ROOT_DIR}/core/AFEditor.h
        ${PUBLIC_HEADER_ROOT_DIR}/platform/mac/AFMacCanvasView.h
    )

    # set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -exported_symbols_list ${CMAKE_CURRENT_LIST_DIR}/arenaforge_editor.lds")
else()

endif()


add_files_by_extension(SRC_FILES ".h;.hpp;.cpp"
    core
    renderer
    drawers
)

if(MACOS)
    add_files_by_extension(MAC_FILES ".h;.hpp;.cpp;.mm"
        core
        renderer
        platform/mac
    )
    list(APPEND SRC_FILES ${MAC_FILES})

    add_files_by_extension(MAC_PUBLIC_HEADER_FILES ".h;.hpp;.cpp;.mm"
        ${PUBLIC_HEADER_ROOT_DIR}/platform/mac
    )
    list(APPEND PUBLIC_HEADER_FILES ${MAC_PUBLIC_HEADER_FILES})
endif()


message(STATUS "PUBLIC_HEADER_FILES: ${PUBLIC_HEADER_FILES}")
message(STATUS "SRC_FILES: ${SRC_FILES}")
message(STATUS "CORE_SHARED_LIBS: ${CORE_SHARED_LIBS}")

add_library(${TAGRGET_NAME} SHARED ${SRC_FILES} ${PUBLIC_HEADER_FILES})

if(NOT BUILD_EDITOR_FRAMEWORK)
    add_library(${TAGRET_NAME_PREFIX}::${TARGET_NAME_SUFFIX} ALIAS ${TAGRGET_NAME})
endif()


add_source_group(PUBLIC_HEADER_FILES ${ArenaForge_ROOT_INCLUDE_DIR}/${TAGRGET_NAME} "PublicHeaders")
add_source_group(SRC_FILES ${CMAKE_CURRENT_LIST_DIR} "Sources")

target_link_libraries(${TAGRGET_NAME}
    ${TAGRET_NAME_PREFIX}::core
)

target_include_directories(${TAGRGET_NAME} PRIVATE
    .
    ${ArenaForge_ROOT_INCLUDE_DIR}
)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug" OR "${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
    target_compile_definitions(${TAGRGET_NAME} PRIVATE DEBUG)
endif ()

if(APPLE)
    set_target_properties(${TAGRGET_NAME} PROPERTIES
      XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC "YES"
      XCODE_ATTRIBUTE_MODULE_VERIFIER_SUPPORTED_LANGUAGES "objective-c objective-c++"
      XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING "YES"
      XCODE_ATTRIBUTE_DEPLOYMENT_POSTPROCESSING[config=Debug] "NO"
      XCODE_ATTRIBUTE_COPY_PHASE_STRIP "YES"
      XCODE_ATTRIBUTE_STRIP_INSTALLED_PRODUCT "YES"
      XCODE_ATTRIBUTE_STRIP_STYLE "non-global"
    )

    target_link_options(${TAGRGET_NAME} PRIVATE -exported_symbols_list "${CMAKE_CURRENT_LIST_DIR}/arenaforge_editor.lds")
endif()

if (BUILD_EDITOR_FRAMEWORK)
    set(EDITOR_FRAMEWORK_IDENTIFIER "com.arenaforge.editor")
    set_target_properties(${TAGRGET_NAME} PROPERTIES
        FRAMEWORK TRUE
        INSTALL_NAME_DIR "@rpath/${TAGRGET_NAME}.framework/${TAGRGET_NAME}"
        INSTALL_RPATH "@executable_path/Frameworks;@loader_path/Frameworks"
        XCODE_ATTRIBUTE_FRAMEWORK_VERSION A
        XCODE_ATTRIBUTE_WRAPPER_EXTENSION "framework"
        XCODE_ATTRIBUTE_DEFINES_MODULE "YES"
        XCODE_ATTRIBUTE_CLANG_ENABLE_MODULES "YES"
        XCODE_ATTRIBUTE_MODULEMAP_FILE "${CMAKE_CURRENT_LIST_DIR}/module.modulemap"
        XCODE_ATTRIBUTE_SKIP_INSTALL "YES"
        XCODE_ATTRIBUTE_INSTALL_PATH "@rpath"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${EDITOR_FRAMEWORK_IDENTIFIER}
        MACOSX_FRAMEWORK_IDENTIFIER ${EDITOR_FRAMEWORK_IDENTIFIER}
        MACOSX_FRAMEWORK_BUNDLE_VERSION "1.0.0"
        MACOSX_FRAMEWORK_SHORT_VERSION_STRING "1.0.0"
        MACOSX_FRAMEWORK_BUNDLE_NAME ${TAGRGET_NAME}
    )

    foreach (header ${PUBLIC_HEADER_FILES})
        file(RELATIVE_PATH dir ${ArenaForge_ROOT_INCLUDE_DIR}/${TAGRGET_NAME} ${header})
        get_filename_component(dir "${dir}" DIRECTORY)
        if (dir)
            set(location "Headers/${dir}")
        else ()
            set(location "Headers")
        endif ()
        set_source_files_properties(${header} PROPERTIES MACOSX_PACKAGE_LOCATION "${location}")
    endforeach ()
endif ()
