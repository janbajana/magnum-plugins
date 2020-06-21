#
# This file is part of Magnum.
#
# Copyright © 2020 Vladimír Vondruš <mosra@centrum.cz> Copyright © 2020 Ján Bajana
# <jan.bajana@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
# associated documentation files (the "Software"), to deal in the Software without restriction,
# including without limitation the rights to use, copy, modify, merge, publish, distribute,
# sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or
# substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
# NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
# OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

find_package(Magnum REQUIRED Video)

find_package(PkgConfig REQUIRED)

pkg_check_modules(gstreamer-1.0 REQUIRED IMPORTED_TARGET gstreamer-1.0)
pkg_check_modules(gstreamer-gl-1.0 REQUIRED IMPORTED_TARGET gstreamer-gl-1.0)
pkg_check_modules(gstreamer-app-1.0 REQUIRED IMPORTED_TARGET gstreamer-app-1.0)
pkg_check_modules(gstreamer-player-1.0 REQUIRED IMPORTED_TARGET gstreamer-player-1.0)

if(BUILD_PLUGINS_STATIC)
    set(MAGNUM_GSTVIDEOIMPORTER_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

# GStVideoImporter plugin
add_plugin(
    GStVideoImporter
    "${MAGNUM_PLUGINS_VIDEOIMPORTER_DEBUG_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_VIDEOIMPORTER_DEBUG_LIBRARY_INSTALL_DIR}"
    "${MAGNUM_PLUGINS_VIDEOIMPORTER_RELEASE_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_VIDEOIMPORTER_RELEASE_LIBRARY_INSTALL_DIR}"
    GStVideoImporter.conf
    GStVideoRenderer.cpp
    GStVideoRenderer.h
    GStVideoImporter.cpp
    GStVideoImporter.h)

if(BUILD_PLUGINS_STATIC AND BUILD_STATIC_PIC)
    set_target_properties(GStVideoImporter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

target_include_directories(GStVideoImporter PUBLIC ${PROJECT_SOURCE_DIR}/src
                                                   ${PROJECT_BINARY_DIR}/src)

target_link_libraries(
    GStVideoImporter PUBLIC Magnum::Video PkgConfig::gstreamer-1.0 PkgConfig::gstreamer-gl-1.0
                            PkgConfig::gstreamer-app-1.0 PkgConfig::gstreamer-player-1.0)

# if(CORRADE_TARGET_WINDOWS)
#     target_link_libraries(GStVideoImporter PUBLIC Magnum::AnyImageImporter)
# elseif(BUILD_PLUGINS_STATIC)
#     target_link_libraries(GStVideoImporter INTERFACE Magnum::AnyImageImporter)
# endif()

# Modify output location only if all are set, otherwise it makes no sense
if(CMAKE_RUNTIME_OUTPUT_DIRECTORY
   AND CMAKE_LIBRARY_OUTPUT_DIRECTORY
   AND CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set_target_properties(
        GStVideoImporter
        PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/magnum$<$<CONFIG:Debug>:-d>/importers
                   LIBRARY_OUTPUT_DIRECTORY
                   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/magnum$<$<CONFIG:Debug>:-d>/importers
                   ARCHIVE_OUTPUT_DIRECTORY
                   ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/magnum$<$<CONFIG:Debug>:-d>/importers)
endif()

install(FILES GStVideoImporter.h ${CMAKE_CURRENT_BINARY_DIR}/configure.h
        DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/GStVideoImporter)

# Automatic static plugin import
if(BUILD_PLUGINS_STATIC)
    install(FILES importStaticPlugin.cpp
            DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/GStVideoImporter)
    target_sources(GStVideoImporter INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/importStaticPlugin.cpp)
endif()

# if(BUILD_TESTS) add_subdirectory(Test) endif()

# MagnumPlugins GStVideoImporter target alias for superprojects
add_library(MagnumPlugins::GStVideoImporter ALIAS GStVideoImporter)
