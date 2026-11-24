cmake_minimum_required(VERSION 3.26)
# set cmake include path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(cmake/uipc_utils.cmake)
include(cmake/uipc_luisa.cmake)

# =========================================================================
# Project Version
# =========================================================================
if(DEFINED SKBUILD_PROJECT_VERSION)
   set(UIPC_VERSION ${SKBUILD_PROJECT_VERSION})
else()
    set(UIPC_VERSION "0.9.0")
endif()
uipc_parse_version(${UIPC_VERSION} UIPC_VERSION_MAJOR UIPC_VERSION_MINOR UIPC_VERSION_PATCH)

# =========================================================================
# Project Options
# =========================================================================
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(_uipc_is_top_level ON)
else()
    set(_uipc_is_top_level OFF)
endif()

option(UIPC_USING_LOCAL_VCPKG "Using local vcpkg" OFF)
option(UIPC_BUILD_PYBIND "Build pyuipc" OFF)
option(UIPC_BUILD_PYTHON_WHEEL "Wheel build mode for Python post-build helper" OFF)
option(UIPC_BUILD_EXAMPLES "Build examples" ${_uipc_is_top_level})
option(UIPC_BUILD_TESTS "Build tests" ${_uipc_is_top_level})
option(UIPC_BUILD_BENCHMARKS "Build benchmarks" ${_uipc_is_top_level})
option(UIPC_ENABLE_INSTALL "Generate CMake install rules" ${_uipc_is_top_level})
option(UIPC_DEV_MODE "Enable developer mode" OFF)
option(UIPC_WITH_USD_SUPPORT "Build with Universal Scene Description (USD) support" OFF)
option(UIPC_WITH_VDB_SUPPORT "Build with Volumetric DataBase (VDB) support" OFF)
option(UIPC_WITH_URDF_SUPPORT "Build with URDF parser support (urdfdom)" OFF)

set(UIPC_PYTHON_EXECUTABLE_PATH "" CACHE PATH "Python executable path")
set(UIPC_USD_INSTALL_DIR "" CACHE PATH
        "USD installation directory: the folder that contains pxrConfig.cmake, e.g. ~/Software/Usd")

# Backend Options
# =========================================================================
option(UIPC_WITH_CUDA_BACKEND "Build with CUDA backend" ON)
option(UIPC_WITH_CUDA_LEGACY_COLLISION
       "Build legacy CUDA broad-phase trajectory filters" ON)
option(UIPC_WITH_LUISA_BACKEND
       "Build with the LuisaCompute backend (requires LuisaCompute; no nvcc)" OFF)
# LuisaCompute source root; its install prefix is expected at <root>/install.
set(LUISA_COMPUTE_ROOT "C:/dev/LuisaCompute" CACHE PATH
    "LuisaCompute source root used by the luisa backend (install at <root>/install)")
# if MAC OS / Android / Emscripten, disable CUDA backend
if(APPLE OR ANDROID OR EMSCRIPTEN)
    # Force OFF
    set(UIPC_WITH_CUDA_BACKEND OFF CACHE BOOL "Build with CUDA backend" FORCE)
endif()

if(UIPC_WITH_CUDA_BACKEND AND NOT DEFINED CMAKE_CUDA_COMPILER)
    if(MSVC AND MSVC_VERSION GREATER_EQUAL 1940 AND DEFINED ENV{CUDA_PATH_V13_0})
        file(TO_CMAKE_PATH "$ENV{CUDA_PATH_V13_0}" _uipc_cuda_v13_root)
        if(EXISTS "${_uipc_cuda_v13_root}/bin/nvcc.exe")
            set(CUDAToolkit_ROOT "${_uipc_cuda_v13_root}" CACHE PATH "CUDA Toolkit root directory" FORCE)
            set(CMAKE_CUDA_COMPILER "${_uipc_cuda_v13_root}/bin/nvcc.exe" CACHE FILEPATH "CUDA compiler" FORCE)
        endif()
        unset(_uipc_cuda_v13_root)
    endif()
endif()

set(UIPC_CUDA_ARCHITECTURES "native" CACHE STRING "Setup CUDA architectures (e.g. \"75,89\" or \"native\")")

# Propagate UIPC_CUDA_ARCHITECTURES to CMake's CUDA variable so generators (VS, etc.)
# produce consistent device code settings instead of falling back to legacy defaults.
if (DEFINED UIPC_CUDA_ARCHITECTURES AND NOT UIPC_CUDA_ARCHITECTURES STREQUAL "")
    # Accept comma- or semicolon-separated lists, normalize to semicolon list
    string(REPLACE "," ";" _uipc_cuda_archs "${UIPC_CUDA_ARCHITECTURES}")
    # Normalize the option itself, not just the CMake variable: the backend
    # targets set their CUDA_ARCHITECTURES property straight from
    # UIPC_CUDA_ARCHITECTURES, and nvcc rejects the comma form with
    # "'89' is not in 'keyword=value' format" at compile time.
    set(UIPC_CUDA_ARCHITECTURES "${_uipc_cuda_archs}" CACHE STRING
        "Setup CUDA architectures (e.g. \"75,89\" or \"native\")" FORCE)
    # Export to CMAKE_CUDA_ARCHITECTURES only when top-level or not already set by parent
    if(_uipc_is_top_level OR NOT DEFINED CMAKE_CUDA_ARCHITECTURES OR CMAKE_CUDA_ARCHITECTURES STREQUAL "")
        set(CMAKE_CUDA_ARCHITECTURES ${_uipc_cuda_archs} CACHE STRING "CUDA architectures" FORCE)
        message(STATUS "Propagated UIPC_CUDA_ARCHITECTURES -> CMAKE_CUDA_ARCHITECTURES: ${CMAKE_CUDA_ARCHITECTURES}")
    endif()
endif()

# Install Options
# =========================================================================
set(UIPC_INSTALL_DIR "." CACHE PATH "Install directory, default is current directory")
if(DEFINED SKBUILD)
    if(NOT SKBUILD EQUAL 2)
        uipc_error("MUST use scikit-build-core(2), but got ${SKBUILD}")
    endif()
    set(UIPC_INSTALL_DIR "uipc/_native")
    set(UIPC_BUILD_PYTHON_WHEEL ON)
endif()

if(UIPC_BUILD_PYTHON_WHEEL)
    set(UIPC_BUILD_PYBIND ON)
endif()

# =========================================================================
# Show Logo and Options
# =========================================================================
if(_uipc_is_top_level)
    uipc_show_logo()
endif()
if(UIPC_USING_LOCAL_VCPKG)
    uipc_find_python_executable_path()
    uipc_show_options()
    uipc_config_vcpkg_install()
else()
    uipc_show_options()
endif()


# =========================================================================
#
# Libuipc Project
#
# =========================================================================

project(uipc VERSION ${UIPC_VERSION})

# Bundled header-only externals (Target-First: reuses parent targets when available)
add_subdirectory(extern)

# The LuisaCompute dependency is resolved at top level (after project()) so the
# imported luisa::compute target is visible to both the backend and its tests.
if(UIPC_WITH_LUISA_BACKEND)
    uipc_luisa_resolve_dependency()
endif()

if(UIPC_BUILD_TESTS)
    enable_testing()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json

# uipc
add_subdirectory(src)

# apps
if(UIPC_BUILD_EXAMPLES OR UIPC_BUILD_TESTS OR UIPC_BUILD_BENCHMARKS)
    add_subdirectory(apps)
endif()

# install headers (libuipc/include)
if(UIPC_ENABLE_INSTALL)
    uipc_install_headers()
endif()
