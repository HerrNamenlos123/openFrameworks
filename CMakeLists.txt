cmake_minimum_required(VERSION 3.24)
project(openframeworks)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/utils.cmake)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(OF_IS_MAIN_PROJECT ON)
else()
    set(OF_IS_MAIN_PROJECT OFF)
endif()

# Here are all the options you can set for customizing your build
option(BUILD_SHARED_LIBS "Build shared libs instead of static ones" OFF)
option(BUILD_EXAMPLES "Add all OpenFrameworks examples to the project" ${OF_IS_MAIN_PROJECT})
option(BUILD_TESTS "Add all OpenFrameworks examples to the project" ${OF_IS_MAIN_PROJECT})

# This defines the openFrameworks library
add_library(openframeworks)
add_library(of::openframeworks ALIAS openframeworks)
add_subdirectory(libs/openFrameworks)

# C++ standard version and disabling non-standard compiler specific features
target_compile_features(openframeworks PUBLIC cxx_std_17)
set_target_properties(openframeworks PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(openframeworks PROPERTIES FOLDER "openframeworks")

# Preprocessor defines
target_compile_definitions(openframeworks PUBLIC 
    OF_USING_STD_FS             # Use the new C++17 filesystem library instead of boost
    UNICODE _UNICODE            # WinAPI setup
    WIN32_LEAN_AND_MEAN         # Prevents Windows.h from adding unnecessary includes
    NOMINMAX                    # Prevents Windows.h from defining min/max as macros
    _CRT_SECURE_NO_WARNINGS     # Prevent MSVC from complaining about memset, strcpy, etc.

    # Remember our dependencies that they are statically linked
    # CURL_STATICLIB
    # POCO_STATIC
    # CAIRO_WIN32_STATIC_BUILD
)

# Compiler flags specific to MSVC
if(MSVC)
    target_compile_options(openframeworks PUBLIC "/Zc:__cplusplus")         # Force MSVC to make __cplusplus macro standards-compliant
    target_compile_options(openframeworks PUBLIC "/Zc:preprocessor")        # Force MSVC to use the standards-compliant preprocessor
    target_compile_options(openframeworks PUBLIC "/MP")                     # Enable multi-core compilation
    target_compile_options(openframeworks PUBLIC "/utf-8")                  # Use UTF-8 source and execution character set
    target_link_options(openframeworks PUBLIC "/ignore:4099")               # Suppress compiler warning that no .pdb file is available for debugging (in third-party libraries)
    target_link_options(openframeworks PUBLIC "/NODEFAULTLIB:LIBCMT")       # Don't link to the default C runtime library (Conflicting with third-party libraries)
    target_link_options(openframeworks PUBLIC "/NODEFAULTLIB:$<IF:$<CONFIG:Debug>,MSVCRT,MSVCRTd>")     # Don't link to the default C++ standard library (Conflicting with third-party libraries)
endif()

# Where compiled binaries should be placed
set_target_properties(openframeworks PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib/$<IF:$<CONFIG:Debug>,debug,release>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib/$<IF:$<CONFIG:Debug>,debug,release>"
)

# OF dependencies
# find_package(assimp REQUIRED)

add_subdirectory(addons)

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
