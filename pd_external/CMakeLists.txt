cmake_minimum_required(VERSION 3.10)
project(songdataselector_pd_external VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set default build type to Release
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Platform-specific settings for the output library name
if(APPLE)
  set(EXTENSION "pd_darwin")
elseif(WIN32)
  set(EXTENSION "dll")
elseif(UNIX)
  set(EXTENSION "pd_linux")
else()
  message(FATAL_ERROR "Platform not supported")
endif()

#search for nlohmann json
find_package(nlohmann_json REQUIRED)

# Create the external library
add_library(songdataselector MODULE
  songdataselector.cpp
  songdataselector.h
)

target_link_libraries(songdataselector PRIVATE nlohmann_json::nlohmann_json)

# Set the output name and properties
set_target_properties(songdataselector PROPERTIES
  PREFIX ""
  OUTPUT_NAME "songdataselector"
  SUFFIX ".${EXTENSION}"
)

# Include Pure Data headers
target_include_directories(songdataselector PRIVATE ${PD_INCLUDE_DIR})

# Platform specific flags and settings
if(APPLE)
  target_link_options(songdataselector PRIVATE -undefined dynamic_lookup -bundle)
elseif(UNIX)
  target_link_options(songdataselector PRIVATE -shared -Wl,--export-dynamic)
endif()

# Set compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(songdataselector PRIVATE -Wall -Wextra -fPIC)
endif()

# Define PD macro
target_compile_definitions(songdataselector PRIVATE PD)

# Installation

# Add strip rule for Release builds
if(CMAKE_BUILD_TYPE MATCHES "Release")
  add_custom_command(TARGET songdataselector POST_BUILD
    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:songdataselector>
  )
endif()

# Output configuration info
message(STATUS "Pure Data include directory: ${PD_INCLUDE_DIR}")
message(STATUS "Installation directory: ${PD_INSTALL_DIR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
