cmake_minimum_required(VERSION 3.17)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Do not build in-source. Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source.")
endif()

project(asd VERSION 0.1.0 LANGUAGES C)

# ---------------------------- Additional Modules ----------------------------
set(CMAKE_MODULE_PATH "${${PROJECT_NAME}_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(GNUInstallDirs) # This will define the default values for installation directories (all platforms even if named GNU)
include(InstallRequiredSystemLibraries) # Tell CMake that the `install` target needs to install required system libraries (eg: Windows SDK)
include(CMakePackageConfigHelpers) # Helper to create relocatable packages
include(GenerateExportHeader) # Helper to generate export macros

# Set default build
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)

    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

add_subdirectory(src) # Library Code
add_subdirectory(apps) # Executable Code