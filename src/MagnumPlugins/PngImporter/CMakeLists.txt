#
#   This file is part of Magnum.
#
#   Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
#               2020 Vladimír Vondruš <mosra@centrum.cz>
#
#   Permission is hereby granted, free of charge, to any person obtaining a
#   copy of this software and associated documentation files (the "Software"),
#   to deal in the Software without restriction, including without limitation
#   the rights to use, copy, modify, merge, publish, distribute, sublicense,
#   and/or sell copies of the Software, and to permit persons to whom the
#   Software is furnished to do so, subject to the following conditions:
#
#   The above copyright notice and this permission notice shall be included
#   in all copies or substantial portions of the Software.
#
#   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#   DEALINGS IN THE SOFTWARE.
#

find_package(Magnum REQUIRED Trade)
find_package(PNG REQUIRED)

if(BUILD_PLUGINS_STATIC)
    set(MAGNUM_PNGIMPORTER_BUILD_STATIC 1)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/configure.h.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/configure.h)

# PngImporter plugin
add_plugin(PngImporter
    "${MAGNUM_PLUGINS_IMPORTER_DEBUG_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_IMPORTER_DEBUG_LIBRARY_INSTALL_DIR}"
    "${MAGNUM_PLUGINS_IMPORTER_RELEASE_BINARY_INSTALL_DIR};${MAGNUM_PLUGINS_IMPORTER_RELEASE_LIBRARY_INSTALL_DIR}"
    PngImporter.conf
    PngImporter.cpp
    PngImporter.h)
if(MAGNUM_PNGIMPORTER_BUILD_STATIC AND BUILD_STATIC_PIC)
    set_target_properties(PngImporter PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
target_include_directories(PngImporter
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
        ${PROJECT_BINARY_DIR}/src
    PRIVATE
        ${PNG_INCLUDE_DIRS})
target_link_libraries(PngImporter PUBLIC
    Magnum::Trade
    ${PNG_LIBRARIES})
# Modify output location only if all are set, otherwise it makes no sense
if(CMAKE_RUNTIME_OUTPUT_DIRECTORY AND CMAKE_LIBRARY_OUTPUT_DIRECTORY AND CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set_target_properties(PngImporter PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/magnum$<$<CONFIG:Debug>:-d>/importers
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/magnum$<$<CONFIG:Debug>:-d>/importers
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/magnum$<$<CONFIG:Debug>:-d>/importers)
endif()

install(FILES PngImporter.h ${CMAKE_CURRENT_BINARY_DIR}/configure.h
    DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/PngImporter)

# Automatic static plugin import
if(MAGNUM_PNGIMPORTER_BUILD_STATIC)
    install(FILES importStaticPlugin.cpp DESTINATION ${MAGNUM_PLUGINS_INCLUDE_INSTALL_DIR}/PngImporter)
    target_sources(PngImporter INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/importStaticPlugin.cpp)
endif()

if(BUILD_TESTS)
    add_subdirectory(Test)
endif()

# MagnumPlugins PngImporter target alias for superprojects
add_library(MagnumPlugins::PngImporter ALIAS PngImporter)
