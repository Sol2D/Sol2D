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

set(SOL2D_USE_GAMES
    OFF
    CACHE BOOL "Enables test games"
)

set(SOL2D_USE_ASAN
    OFF
    CACHE BOOL "Enables address sanitizer (ASAN)"
)

set(SOL2D_USE_CPPCHECK
    OFF
    CACHE BOOL "Enables static code analyzer (cppcheck)"
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
set(THIRD_PATY_DIR ${CMAKE_CURRENT_LIST_DIR}/third_party)

file(GLOB_RECURSE SOL2D_SRC
    "${SOL2D_SRC_DIR}/*.h"
    "${SOL2D_SRC_DIR}/*.cpp"
)

add_subdirectory(third_party/SDL)
add_subdirectory(third_party/SDL_image)
add_subdirectory(third_party/SDL_ttf)
add_subdirectory(third_party/SDL_mixer)
add_subdirectory(third_party/box2d)

find_package(ZLIB REQUIRED)
find_package(tinyxml2 REQUIRED)
find_package(spdlog REQUIRED)
find_package(Boost REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED lua)
pkg_check_modules(ZSTD REQUIRED libzstd)

if(SOL2D_USE_CPPCHECK)
    find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
    list(APPEND CMAKE_CXX_CPPCHECK
        "--suppress=*:/third-party/*"
        "--suppress=unusedFunction"
        "--suppress=missingIncludeSystem"
        "--enable=all"
        "--inline-suppr"
    )
else()
    set(CMAKE_CXX_CPPCHECK "")
endif()

add_executable(${PROJECT_NAME} ${SOL2D_SRC})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
    if(SOL2D_USE_ASAN)
        target_link_options(${PROJECT_NAME} PRIVATE /fsanitize=address)
    endif()
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    if(SOL2D_USE_ASAN)
        target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
    endif()
endif()

target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/src
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/third_party/SDL/include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/third_party/SDL_image/include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/third_party/SDL_ttf/include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/third_party/SDL_mixer/include
    PRIVATE ${CMAKE_CURRENT_LIST_DIR}/third_party/box2d/include
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

if(SOL2D_USE_GAMES)
    set(SOL2D_GAMES_DIR_NAME games)
    set(SOL2D_GAMES_DIR
        "${CMAKE_CURRENT_LIST_DIR}/${SOL2D_GAMES_DIR_NAME}"
    )
    set(SOL2D_GAME_CONFING
        "${CMAKE_CURRENT_LIST_DIR}/game.xml"
        "${CMAKE_CURRENT_LIST_DIR}/game_rpg.xml"
    )
    file(GLOB_RECURSE SOL2D_GAMES_SRC
        "${SOL2D_GAMES_DIR}/*"
    )
    list(APPEND SOL2D_GAMES_SRC
        ${SOL2D_GAME_CONFING}
    )
    add_custom_target(copy_games_directories ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${SOL2D_GAMES_DIR}
        "${PROJECT_BINARY_DIR}/${SOL2D_GAMES_DIR_NAME}"
        COMMENT "Copying the ${SOL2D_GAMES_DIR_NAME}"
    )
    add_custom_target(copy_game_config ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SOL2D_GAME_CONFING}
        ${PROJECT_BINARY_DIR}
        COMMENT "Copying the ${SOL2D_GAME_CONFING}"
    )
    add_dependencies(${PROJECT_NAME}
        copy_games_directories
        copy_game_config
    )
    add_custom_target(games SOURCES
        ${SOL2D_GAMES_SRC}
    )
endif(SOL2D_USE_GAMES)

add_custom_target(misc SOURCES
    .gitignore
    LICENSE.txt
    README.md
)
