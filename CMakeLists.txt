# Sol2D Game Engine
# Copyright (C) 2023-2025 Sergey Smolyannikov aka brainstream
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

cmake_minimum_required(VERSION 3.30)

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
set(SOL2D_SHADERS_DIR ${SOL2D_SRC_DIR}/Shaders)
set(THIRD_PATY_DIR ${CMAKE_CURRENT_LIST_DIR}/third_party)

file(GLOB_RECURSE SOL2D_SRC
    "${SOL2D_SRC_DIR}/*.h"
    "${SOL2D_SRC_DIR}/*.cpp"
)

file(GLOB_RECURSE SOL2D_SHADERS_SRC
    "${SOL2D_SHADERS_DIR}/*.vert"
    "${SOL2D_SHADERS_DIR}/*.frag"
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
find_package(Vulkan COMPONENTS glslc REQUIRED)
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

target_precompile_headers(${PROJECT_NAME}
    PRIVATE ${SOL2D_SRC_DIR}/MediaLayer.h
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

block() # Shaders
    set(SHADER_OUTPUTS)
    set(SHADER_OUTDIR "${CMAKE_CURRENT_BINARY_DIR}/Shaders")

    foreach(SHADER_SRC IN LISTS SOL2D_SHADERS_SRC)
        cmake_path(GET SHADER_SRC FILENAME SHADER_NAME)
        set(SHADER_DEST "${SHADER_OUTDIR}/${SHADER_NAME}.spv")
        file(RELATIVE_PATH SHADER_DEST_RELATIVE_PATH ${CMAKE_CURRENT_BINARY_DIR} ${SHADER_DEST})

        set(GLSLC_COMMAND)
        list(APPEND GLSLC_COMMAND Vulkan::glslc)
        list(APPEND GLSLC_COMMAND ${SHADER_SRC})
        if(CMAKE_BUILD_TYPE STREQUAL "Release")
            list(APPEND GLSLC_COMMAND "-O")
        elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
            list(APPEND GLSLC_COMMAND "-O0")
            list(APPEND GLSLC_COMMAND "-g")
        endif()
        list(APPEND GLSLC_COMMAND "-o")
        list(APPEND GLSLC_COMMAND ${SHADER_DEST})
        list(APPEND SHADER_OUTPUTS ${SHADER_DEST})

        add_custom_command(
            OUTPUT ${SHADER_DEST}
            COMMAND ${GLSLC_COMMAND}
            DEPENDS ${SHADER_SRC}
            COMMENT "Compiling SPIR-V ${SHADER_DEST_RELATIVE_PATH}"
        )
    endforeach()

    add_custom_target(shader_out_dir ALL
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SHADER_OUTDIR}"
        COMMENT "Creating a shader output directory ${SHADER_OUTDIR}"
    )

    add_custom_target(shaders ALL
        SOURCES ${SOL2D_SHADERS_SRC}
        DEPENDS shader_out_dir ${SHADER_OUTPUTS}
    )

    add_dependencies(${PROJECT_NAME} shaders)
endblock()

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
