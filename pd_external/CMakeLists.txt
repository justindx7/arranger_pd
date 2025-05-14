cmake_minimum_required(VERSION 3.10)
project(helloworld_pd_external VERSION 1.0.0 LANGUAGES CXX)

# Use C++11 standard
set(CMAKE_CXX_STANDARD 11)
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


# Create the external library
add_library(helloworld MODULE
  helloworld.cpp
  helloworld.h
)

# Set the output name and properties
set_target_properties(helloworld PROPERTIES
  PREFIX ""
  OUTPUT_NAME "helloworld"
  SUFFIX ".${EXTENSION}"
)

# Include Pure Data headers
target_include_directories(helloworld PRIVATE ${PD_INCLUDE_DIR})

# Platform specific flags and settings
if(APPLE)
  target_link_options(helloworld PRIVATE -undefined dynamic_lookup -bundle)
elseif(UNIX)
  target_link_options(helloworld PRIVATE -shared -Wl,--export-dynamic)
endif()

# Set compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(helloworld PRIVATE -Wall -Wextra -fPIC)
endif()

# Define PD macro
target_compile_definitions(helloworld PRIVATE PD)

# Installation

# Add strip rule for Release builds
if(CMAKE_BUILD_TYPE MATCHES "Release")
  add_custom_command(TARGET helloworld POST_BUILD
    COMMAND ${CMAKE_STRIP} $<TARGET_FILE:helloworld>
  )
endif()

# Output configuration info
message(STATUS "Pure Data include directory: ${PD_INCLUDE_DIR}")
message(STATUS "Installation directory: ${PD_INSTALL_DIR}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
