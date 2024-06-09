# Sol2D Game Engine
# Copyright (C) 2023-2024 Sergey Smolyannikov aka brainstream
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU Lesser General Public License as published by the Free
# Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Lesser Public License for more
# details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.24)

project(sol2d
    LANGUAGES CXX
    VERSION 0.0.0
)

if(LINUX)
    list(APPEND CMAKE_MODULE_PATH "/usr/lib/x86_64-linux-gnu/cmake/")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_SKIP_BUILD_RPATH ON)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Application will be built in the Debug configuration")
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

set(SOL2D_SRC_DIR ${CMAKE_CURRENT_LIST_DIR}/src/Sol2D)
set(SOL2D_WORKSPACE_DIR ${CMAKE_CURRENT_LIST_DIR}/workspace)
set(THIRD_PATY_DIR ${CMAKE_CURRENT_LIST_DIR}/third_party)

file(GLOB_RECURSE SOL2D_SRC
    "${SOL2D_SRC_DIR}/*.h"
    "${SOL2D_SRC_DIR}/*.cpp"
)

file(GLOB_RECURSE SOL2D_WORKSPACE
    "${SOL2D_WORKSPACE_DIR}/*"
)

source_group("Engine" ${SOL2D_SRC_DIR})
source_group("Workspace" ${SOL2D_WORKSPACE_DIR})

add_subdirectory(third_party/SDL)
add_subdirectory(third_party/SDL_image)
add_subdirectory(third_party/SDL_ttf)
add_subdirectory(third_party/SDL_mixer)

find_package(ZLIB REQUIRED)
find_package(box2d REQUIRED)
find_package(tinyxml2 REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED lua)
pkg_check_modules(ZSTD REQUIRED libzstd)

add_executable(${PROJECT_NAME} ${SOL2D_SRC})

target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/third_party/SDL/include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/third_party/SDL_image/include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/third_party/SDL_ttf/include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/third_party/SDL_mixer/include
    PRIVATE ${ZSTD_INCLUDEDIR}
    PRIVATE ${SDL3_INCLUDE_DIR}
    PRIVATE ${LUA_INCLUDEDIR}
    PRIVATE ${LUA_INCLUDE_DIRS}
    PRIVATE ${ZSTD_INCLUDE_DIRS}
    PRIVATE ${Boost_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    ZLIB::ZLIB
    SDL3::SDL3
    SDL3_image::SDL3_image
    SDL3_ttf::SDL3_ttf
    SDL3_mixer::SDL3_mixer
    tinyxml2::tinyxml2
    box2d::box2d
    spdlog::spdlog
    Boost::boost
    ${LUA_LINK_LIBRARIES}
    ${ZSTD_LINK_LIBRARIES}
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

add_custom_target(copy_workspace ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_LIST_DIR}/workspace
    ${PROJECT_BINARY_DIR}/workspace
    COMMENT "Copying workspace into binary directory"
)

add_dependencies(${PROJECT_NAME} copy_workspace)

add_custom_target(workspace SOURCES ${SOL2D_WORKSPACE})

add_custom_target(misc SOURCES
    .gitignore
    LICENSE.txt
    README.md
)
