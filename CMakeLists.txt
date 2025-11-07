#[[ This is a cmake configuration file for a cross-platform 2D game engine project
designed to work on Windows, macOS, and Linux. It manages dependencies using
vcpkg on Windows and system package managers on macOS and Linux. It also includes
a testing setup using CTest. ]]

cmake_minimum_required(VERSION 3.20)

# Verify CMake version
if(${CMAKE_VERSION} VERSION_LESS 3.20)
    message(FATAL_ERROR "CMake version ${CMAKE_VERSION} is too old. Version 3.20 or higher is required.")
endif()

# Set C++ standard (prefer per-target compile features for strictness later)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Project declaration (single canonical project() call)
project(2DGameEngine VERSION 0.1.0
        DESCRIPTION "A cross-platform 2D game engine"
        LANGUAGES CXX)

# Note: CMAKE_TOOLCHAIN_FILE must be provided on the initial configure
# (e.g. -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake).
# Setting it here AFTER the initial configure has no effect on current run.

# Optional: auto-generate a Visual Studio 2022 solution on Windows
option(AUTO_GEN_VS "Auto-generate a Visual Studio 17 2022 solution in vs_build" ON)

if(WIN32 AND AUTO_GEN_VS)
    if(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
        set(VS_BUILD_DIR "${CMAKE_SOURCE_DIR}/vs_build")
        if(NOT EXISTS "${VS_BUILD_DIR}/CMakeCache.txt")
            message(STATUS "Auto-generating __Visual Studio 17 2022__ solution into: ${VS_BUILD_DIR}")
            execute_process(
                COMMAND "${CMAKE_COMMAND}" -S "${CMAKE_SOURCE_DIR}" -B "${VS_BUILD_DIR}"
                        -G "Visual Studio 17 2022" -A x64
                RESULT_VARIABLE _cmake_vs_res
                OUTPUT_VARIABLE _cmake_vs_out
                ERROR_VARIABLE _cmake_vs_err
            )
            if(NOT _cmake_vs_res EQUAL 0)
                message(WARNING "Failed to generate Visual Studio solution:\n${_cmake_vs_err}")
            else()
                message(STATUS "Generated Visual Studio solution at ${VS_BUILD_DIR}/${PROJECT_NAME}.sln")
            endif()
            message(FATAL_ERROR
                "Auto-bootstrap created the Visual Studio build tree. Open ${VS_BUILD_DIR}/${PROJECT_NAME}.sln "
                "or re-run CMake with the __Visual Studio 17 2022__ generator.")
        endif()
    endif()
endif()

# ------------------------
# vcpkg / Windows options
# ------------------------
option(VCPKG_AUTO_BOOTSTRAP "Automatically clone and bootstrap vcpkg during configure (not recommended)" OFF)
option(VCPKG_AUTO_INSTALL_PACKAGES "Automatically run `vcpkg install` for required packages during configure (not recommended)" OFF)
set(VCPKG_DIR "${CMAKE_SOURCE_DIR}/vcpkg" CACHE PATH "Path to vcpkg root")
set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "vcpkg triplet")

if(WIN32 AND VCPKG_AUTO_BOOTSTRAP)
    find_package(Git REQUIRED)
    if(NOT EXISTS "${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake")
        message(STATUS "Bootstrapping vcpkg into ${VCPKG_DIR} (VCPKG_AUTO_BOOTSTRAP=ON)")
        execute_process(COMMAND ${GIT_EXECUTABLE} clone https://github.com/microsoft/vcpkg.git ${VCPKG_DIR}
                        RESULT_VARIABLE _git_res OUTPUT_QUIET ERROR_VARIABLE _git_err)
        if(NOT _git_res EQUAL 0)
            message(FATAL_ERROR "Failed to clone vcpkg: ${_git_err}")
        endif()
        execute_process(COMMAND "${VCPKG_DIR}/bootstrap-vcpkg.bat" WORKING_DIRECTORY "${VCPKG_DIR}"
                        RESULT_VARIABLE _boot_res OUTPUT_QUIET ERROR_VARIABLE _boot_err)
        if(NOT _boot_res EQUAL 0)
            message(FATAL_ERROR "Failed to bootstrap vcpkg: ${_boot_err}")
        endif()
    endif()
endif()

# If vcpkg toolchain is available in repo, allow use (informational only).
if(WIN32 AND EXISTS "${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake")
    list(APPEND CMAKE_PREFIX_PATH "${VCPKG_DIR}/installed/${VCPKG_TARGET_TRIPLET}/share")
    message(STATUS "Detected vcpkg at ${VCPKG_DIR}; you should configure with: -DCMAKE_TOOLCHAIN_FILE=${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake")
else()
    if(WIN32)
        message(STATUS "vcpkg toolchain not found in ${VCPKG_DIR}. If you use vcpkg, run CMake with -DCMAKE_TOOLCHAIN_FILE=path/to/vcpkg/scripts/buildsystems/vcpkg.cmake")
    endif()
endif()

# Only run installs if explicitly requested (dangerous to run every configure)
if(WIN32 AND VCPKG_AUTO_INSTALL_PACKAGES)
    if(NOT EXISTS "${VCPKG_DIR}/vcpkg.exe")
        message(FATAL_ERROR "vcpkg executable not found at ${VCPKG_DIR}/vcpkg.exe. Bootstrap may have failed or VCPKG_DIR is incorrect.")
    endif()
    set(VCPKG_REQUIRED_PACKAGES zlib libpng SDL2 SDL2-ttf SDL2-image SDL2-mixer lua imgui glm)
    foreach(pkg ${VCPKG_REQUIRED_PACKAGES})
        message(STATUS "vcpkg install: ${pkg}")
        execute_process(COMMAND "${VCPKG_DIR}/vcpkg.exe" install ${pkg}:${VCPKG_TARGET_TRIPLET}
                        RESULT_VARIABLE _vpk_res OUTPUT_VARIABLE _vpk_out ERROR_VARIABLE _vpk_err
                        WORKING_DIRECTORY "${VCPKG_DIR}")
        if(NOT _vpk_res EQUAL 0)
            message(FATAL_ERROR "vcpkg install failed for ${pkg}:\n${_vpk_err}")
        endif()
    endforeach()
endif()

# ------------------------
# Platform-specific configuration
# ------------------------
if(WIN32)
    # Find packages (expect vcpkg toolchain or installed packages available)
    find_package(ZLIB REQUIRED)
    find_package(PNG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_mixer CONFIG REQUIRED)
    find_package(Lua REQUIRED)
    find_package(imgui CONFIG REQUIRED)
    find_package(glm CONFIG REQUIRED)
    find_package(sol2 CONFIG QUIET)

    if(NOT sol2_FOUND)
        include(FetchContent)
        FetchContent_Declare(
                sol2
                GIT_REPOSITORY https://github.com/ThePhD/sol2.git
                GIT_TAG        v3.3.0
        )
        FetchContent_MakeAvailable(sol2)
    endif()

    # Collect source files
    file(GLOB SOURCE_FILES CONFIGURE_DEPENDS
            "${CMAKE_SOURCE_DIR}/src/*.c"
            "${CMAKE_SOURCE_DIR}/src/*.cpp"
            "${CMAKE_SOURCE_DIR}/src/**/*.c"
            "${CMAKE_SOURCE_DIR}/src/**/*.cpp"
    )
    message(STATUS "Source files: ${SOURCE_FILES}")

    ##################################################################################
    ## TESTING ##
    enable_testing()

    add_library(MyLoggerLib STATIC
            src/Logger/Logger.cpp
    )

    target_include_directories(MyLoggerLib PUBLIC
            "${CMAKE_SOURCE_DIR}/src/Logger"
    )

    add_executable(LoggerTest
            tests/LoggerTest.cpp
    )

    target_link_libraries(LoggerTest PRIVATE MyLoggerLib)

    add_test(NAME LoggerTest COMMAND LoggerTest)
    ##################################################################################

    add_executable(${PROJECT_NAME} ${SOURCE_FILES})

    # Link libraries using imported targets where possible
    target_link_libraries(${PROJECT_NAME} PRIVATE
            SDL2::SDL2 SDL2::SDL2main
            SDL2_ttf::SDL2_ttf
            SDL2_image::SDL2_image
            SDL2_mixer::SDL2_mixer
            imgui::imgui
            glm::glm
            ZLIB::ZLIB
            PNG::PNG
    )

    # Prefer linking Lua via target if available
    if (TARGET Lua::Lua)
        target_link_libraries(${PROJECT_NAME} PRIVATE Lua::Lua)
    else()
        target_link_libraries(${PROJECT_NAME} PRIVATE ${LUA_LIBRARIES})
    endif()

    # sol2 target if available
    if(TARGET sol2::sol2)
        target_link_libraries(${PROJECT_NAME} PRIVATE sol2::sol2)
    endif()

    target_include_directories(${PROJECT_NAME} PRIVATE
            "${CMAKE_SOURCE_DIR}/src"
            ${LUA_INCLUDE_DIR}
    )

    # Add a build-time copy target for assets
    add_custom_target(copy_assets ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copying assets to binary directory")
    add_dependencies(${PROJECT_NAME} copy_assets)
endif () # WIN32

# macOS-specific dependency management with Homebrew
if(APPLE)
    execute_process(
            COMMAND brew --prefix
            OUTPUT_VARIABLE BREW_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/sdl2/lib/cmake/SDL2")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/sdl2_ttf/lib/cmake/SDL2_ttf")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/sdl2_image/lib/cmake/SDL2_image")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/sdl2_mixer/lib/cmake/SDL2_mixer")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/lua")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/glm")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/imgui")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/libpng")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/zlib")

    message(STATUS "Homebrew prefix: ${BREW_PREFIX}")
    message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
endif()

if(UNIX AND NOT APPLE)
    set(REQUIRED_PACKAGES
            build-essential
            libsdl2-dev
            libsdl2-image-dev
            libsdl2-ttf-dev
            libsdl2-mixer-dev
            libglm-dev
            liblua5.3-dev
            nlohmann-json3-dev
            sol2-dev
    )

    find_package(PkgConfig REQUIRED)

    # Check for system-installed dependencies
    set(MISSING_PACKAGES "")
    pkg_check_modules(SDL2 QUIET sdl2)
    if(SDL2_FOUND)
        message(STATUS "Found sdl2: version ${SDL2_VERSION}")
    else()
        list(APPEND MISSING_PACKAGES "libsdl2-dev")
    endif()
    pkg_check_modules(SDL2_IMAGE QUIET SDL2_image)
    if(SDL2_IMAGE_FOUND)
        message(STATUS "Found SDL2_image: version ${SDL2_IMAGE_VERSION}")
    else()
        list(APPEND MISSING_PACKAGES "libsdl2-image-dev")
    endif()
    pkg_check_modules(SDL2_TTF QUIET SDL2_ttf)
    if(SDL2_TTF_FOUND)
        message(STATUS "Found SDL2_ttf: version ${SDL2_TTF_VERSION}")
    else()
        list(APPEND MISSING_PACKAGES "libsdl2-ttf-dev")
    endif()
    pkg_check_modules(SDL2_MIXER QUIET SDL2_mixer)
    if(SDL2_MIXER_FOUND)
        message(STATUS "Found SDL2_mixer: version ${SDL2_MIXER_VERSION}")
    else()
        list(APPEND MISSING_PACKAGES "libsdl2-mixer-dev")
    endif()
    pkg_check_modules(GLM QUIET glm)
    if(GLM_FOUND)
        message(STATUS "Found glm: version ${GLM_VERSION}")
    else()
        list(APPEND MISSING_PACKAGES "libglm-dev")
    endif()
    pkg_check_modules(LUA QUIET lua5.3)
    if(LUA_FOUND)
        message(STATUS "Found lua5.3: version ${LUA_VERSION}")
    else()
        list(APPEND MISSING_PACKAGES "liblua5.3-dev")
    endif()

    find_program(GCC gcc)
    find_program(GXX g++)
    if(NOT GCC OR NOT GXX)
        list(APPEND MISSING_PACKAGES "build-essential")
    else()
        message(STATUS "Found gcc: ${GCC}")
        message(STATUS "Found g++: ${GXX}")
    endif()

    include(FetchContent)
    FetchContent_Declare(
            imgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG v1.91.4
    )
    FetchContent_MakeAvailable(imgui)

    if(MISSING_PACKAGES)
        string(REPLACE ";" " " MISSING_PACKAGES_STR "${MISSING_PACKAGES}")
        message(WARNING "The following system packages are missing: ${MISSING_PACKAGES_STR}")
        message(FATAL_ERROR
                "Required packages are missing. Install them with:\n"
                "  sudo apt-get update\n"
                "  sudo apt-get install -y ${MISSING_PACKAGES_STR}\n"
                "Then re-run CMake.")
    endif()

    # Collect source files and create target (UNIX)
    file(GLOB SOURCE_FILES CONFIGURE_DEPENDS
            "${CMAKE_SOURCE_DIR}/src/*.c"
            "${CMAKE_SOURCE_DIR}/src/*.cpp"
            "${CMAKE_SOURCE_DIR}/src/**/*.c"
            "${CMAKE_SOURCE_DIR}/src/**/*.cpp"
    )
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})

    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${SDL2_LIBRARIES}
            ${SDL2_IMAGE_LIBRARIES}
            ${SDL2_TTF_LIBRARIES}
            ${SDL2_MIXER_LIBRARIES}
            ${GLM_LIBRARIES}
            ${LUA_LIBRARIES}
    )

    target_include_directories(${PROJECT_NAME} PRIVATE
            "${CMAKE_SOURCE_DIR}/src"
            ${SDL2_INCLUDE_DIRS}
            ${SDL2_IMAGE_INCLUDE_DIRS}
            ${SDL2_TTF_INCLUDE_DIRS}
            ${SDL2_MIXER_INCLUDE_DIRS}
            ${GLM_INCLUDE_DIRS}
            ${LUA_INCLUDE_DIRS}
            ${imgui_SOURCE_DIR}
    )
endif()