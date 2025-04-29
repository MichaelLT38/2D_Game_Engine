cmake_minimum_required(VERSION 3.20)

# Verify CMake version
if(${CMAKE_VERSION} VERSION_LESS 3.20)
    message(FATAL_ERROR "CMake version ${CMAKE_VERSION} is too old. Version 3.20 or higher is required.")
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32)
    # Download and set up vcpkg
    set(VCPKG_DIR "${CMAKE_SOURCE_DIR}/vcpkg")
    if(NOT EXISTS "${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake")
        message(STATUS "vcpkg directory not found at ${VCPKG_DIR}. Installing vcpkg...")
        find_package(Git REQUIRED)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} clone https://github.com/microsoft/vcpkg.git ${VCPKG_DIR}
                RESULT_VARIABLE GIT_CLONE_RESULT
                OUTPUT_VARIABLE GIT_CLONE_OUTPUT
                ERROR_VARIABLE GIT_CLONE_ERROR
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
        if(NOT GIT_CLONE_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to clone vcpkg. Git output: ${GIT_CLONE_OUTPUT}, Error: ${GIT_CLONE_ERROR}")
        endif()
        execute_process(
                COMMAND ${VCPKG_DIR}/bootstrap-vcpkg.bat
                RESULT_VARIABLE BOOTSTRAP_RESULT
                OUTPUT_VARIABLE BOOTSTRAP_OUTPUT
                ERROR_VARIABLE BOOTSTRAP_ERROR
                WORKING_DIRECTORY ${VCPKG_DIR}
        )
        if(NOT BOOTSTRAP_RESULT EQUAL 0)
            message(FATAL_ERROR "Failed to bootstrap vcpkg. Output: ${BOOTSTRAP_OUTPUT}, Error: ${BOOTSTRAP_ERROR}")
        endif()
    endif()
    set(CMAKE_TOOLCHAIN_FILE "${VCPKG_DIR}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
    set(VCPKG_TARGET_TRIPLET x64-windows CACHE STRING "Vcpkg target triplet")

    # Verify vcpkg executable exists
    if(NOT EXISTS "${VCPKG_DIR}/vcpkg.exe")
        message(FATAL_ERROR "vcpkg executable not found at ${VCPKG_DIR}/vcpkg.exe. Bootstrap may have failed.")
    endif()

    # Explicitly add vcpkg installed directory to CMAKE_PREFIX_PATH
    list(APPEND CMAKE_PREFIX_PATH "${VCPKG_DIR}/installed/x64-windows/share")

    # Debug toolchain and paths
    message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")
    message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")

    # Define project
    project(2DGameEngine LANGUAGES CXX)

    # Automatically install required vcpkg packages
    set(VCPKG_REQUIRED_PACKAGES zlib libpng SDL2 SDL2-ttf SDL2-image SDL2-mixer lua imgui glm)
    foreach(PACKAGE ${VCPKG_REQUIRED_PACKAGES})
        execute_process(
                COMMAND ${VCPKG_DIR}/vcpkg.exe install ${PACKAGE}:${VCPKG_TARGET_TRIPLET}
                RESULT_VARIABLE VCPKG_INSTALL_RESULT
                OUTPUT_VARIABLE VCPKG_INSTALL_OUTPUT
                ERROR_VARIABLE VCPKG_INSTALL_ERROR
                WORKING_DIRECTORY ${VCPKG_DIR}
        )
        if(NOT VCPKG_INSTALL_RESULT EQUAL 0)
            message(FATAL_ERROR
                    "Failed to install vcpkg package '${PACKAGE}' for triplet '${VCPKG_TARGET_TRIPLET}'.\n"
                    "Output:\n${VCPKG_INSTALL_OUTPUT}\nError:\n${VCPKG_INSTALL_ERROR}"
            )
        endif()
        message(STATUS "Installed vcpkg package '${PACKAGE}' for triplet '${VCPKG_TARGET_TRIPLET}'")
    endforeach()


    # Find vcpkg-installed packages
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
                GIT_TAG        v3.3.0 # Adjust to desired version
        )
        FetchContent_MakeAvailable(sol2)
        # The FetchContent approach creates a target named 'sol2' or 'sol2::sol2'
        # depending on how the sol2 CMakeLists is structured (some versions
        # may just provide headers). You might need to adjust as required.
    endif()


    # Collect source files
    file(GLOB SOURCE_FILES CONFIGURE_DEPENDS
            "${CMAKE_SOURCE_DIR}/src/*.c"
            "${CMAKE_SOURCE_DIR}/src/*.cpp"
            "${CMAKE_SOURCE_DIR}/src/**/*.c"
            "${CMAKE_SOURCE_DIR}/src/**/*.cpp"
    )
    message(STATUS "Source files: ${SOURCE_FILES}")

    # Create executable
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})

    # Link libraries
    target_link_libraries(${PROJECT_NAME} PRIVATE
            SDL2::SDL2 SDL2::SDL2main
            SDL2_ttf::SDL2_ttf
            SDL2_image::SDL2_image
            SDL2_mixer::SDL2_mixer
            ${LUA_LIBRARIES}
            imgui::imgui
            glm::glm
            ZLIB::ZLIB
            PNG::PNG
            sol2::sol2
    )

    # Include directories
    target_include_directories(${PROJECT_NAME} PRIVATE
            "${CMAKE_SOURCE_DIR}/src"
            ${LUA_INCLUDE_DIR}
    )
    file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
endif ()

# macOS-specific dependency management with Homebrew
if(APPLE)
    # Find Homebrew prefix
    execute_process(
            COMMAND brew --prefix
            OUTPUT_VARIABLE BREW_PREFIX
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    # Set paths for Homebrew-installed libraries
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/sdl2/lib/cmake/SDL2")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/sdl2_ttf/lib/cmake/SDL2_ttf")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/sdl2_image/lib/cmake/SDL2_image")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/sdl2_mixer/lib/cmake/SDL2_mixer")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/lua")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/glm")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/imgui")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/libpng")
    list(APPEND CMAKE_PREFIX_PATH "${BREW_PREFIX}/opt/zlib")

    # Debug paths
    message(STATUS "Homebrew prefix: ${BREW_PREFIX}")
    message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
endif()

if(UNIX AND NOT APPLE)
    # Define required system packages
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

    # Find pkg-config
    find_package(PkgConfig REQUIRED)
    if(NOT PkgConfig_FOUND)
        list(APPEND MISSING_PACKAGES "pkgconf")
    else()
        # Debug: Show pkg-config search path
        execute_process(COMMAND pkg-config --variable pc_path pkg-config
                OUTPUT_VARIABLE PKG_CONFIG_PATH
                OUTPUT_STRIP_TRAILING_WHITESPACE)
        message(STATUS "pkg-config search path: ${PKG_CONFIG_PATH}")
    endif()

    # Check for system-installed dependencies without failing immediately
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

    # Check for build-essential (basic check for gcc/g++)
    find_program(GCC gcc)
    find_program(GXX g++)
    if(NOT GCC OR NOT GXX)
        list(APPEND MISSING_PACKAGES "build-essential")
    else()
        message(STATUS "Found gcc: ${GCC}")
        message(STATUS "Found g++: ${GXX}")
    endif()

    # Include ImGui (header-only, fetch via FetchContent)
    include(FetchContent)
    FetchContent_Declare(
            imgui
            GIT_REPOSITORY https://github.com/ocornut/imgui.git
            GIT_TAG v1.91.4
    )
    FetchContent_MakeAvailable(imgui)

    # Handle missing packages
    if(MISSING_PACKAGES)
        # Join package list with spaces for apt-get
        string(REPLACE ";" " " MISSING_PACKAGES_STR "${MISSING_PACKAGES}")
        message(WARNING "The following system packages are missing: ${MISSING_PACKAGES_STR}")
        message(FATAL_ERROR
                "Required packages are missing. Install them with:\n"
                "  sudo apt-get update\n"
                "  sudo apt-get install -y ${MISSING_PACKAGES_STR}\n"
                "Then re-run CMake."
        )
    endif()

    # Define project
    project(2DGameEngine LANGUAGES CXX)

    # Collect source files
    file(GLOB SOURCE_FILES CONFIGURE_DEPENDS
            "${CMAKE_SOURCE_DIR}/src/*.c"
            "${CMAKE_SOURCE_DIR}/src/*.cpp"
            "${CMAKE_SOURCE_DIR}/src/**/*.c"
            "${CMAKE_SOURCE_DIR}/src/**/*.cpp"
    )

    # Create executable
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})

    # Link libraries using pkg-config results
    target_link_libraries(${PROJECT_NAME} PRIVATE
            ${SDL2_LIBRARIES}
            ${SDL2_IMAGE_LIBRARIES}
            ${SDL2_TTF_LIBRARIES}
            ${SDL2_MIXER_LIBRARIES}
            ${GLM_LIBRARIES}
            ${LUA_LIBRARIES}
    )

    # Include directories using pkg-config results
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