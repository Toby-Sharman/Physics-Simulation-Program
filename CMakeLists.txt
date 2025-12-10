# ==================================================
# Project: Simulation_program
# Description: OPM simulation program
# CMake version required: 3.30
# ==================================================
cmake_minimum_required(VERSION 3.30)
project(Simulation_program)

# -------------------------
# Compiler settings
# -------------------------
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON) # Link time optimisation

# -------------------------
# Default build type
# -------------------------
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the build type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")

# -------------------------
# Build/output locations (pulled from config/path_config.h)
# -------------------------
set(PATH_CONFIG_FILE "${CMAKE_SOURCE_DIR}/config/path_config.h")
if(NOT EXISTS "${PATH_CONFIG_FILE}")
    message(FATAL_ERROR "Missing config/path_config.h; please create it with path definitions.")
endif()

function(read_path_constant OUT_VAR CONST_NAME)
    file(STRINGS "${PATH_CONFIG_FILE}" _line REGEX "inline constexpr std::string_view ${CONST_NAME} = \"")
    if(NOT _line)
        message(FATAL_ERROR "Could not find constant '${CONST_NAME}' in ${PATH_CONFIG_FILE}")
    endif()
    string(REGEX MATCH "\"([^\"]+)\"" _match "${_line}")
    if(NOT CMAKE_MATCH_1)
        message(FATAL_ERROR "Failed to parse value for '${CONST_NAME}' in ${PATH_CONFIG_FILE}")
    endif()
    set(${OUT_VAR} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

read_path_constant(PROGRAM_BUILD_DIR "buildDirectory")
read_path_constant(PROGRAM_OUTPUT_DIR "outputDirectory")
read_path_constant(PROGRAM_DATABASE_DIR "databaseDirectory")
read_path_constant(MATERIAL_DATABASE_JSON "materialDatabaseJson")
read_path_constant(PARTICLE_DATABASE_JSON "particleDatabaseJson")
read_path_constant(MATERIAL_DATABASE_PATH "materialDatabasePath")
read_path_constant(PARTICLE_DATABASE_PATH "particleDatabasePath")

# Normalize build directory against project root, then others against build directory
if(NOT IS_ABSOLUTE "${PROGRAM_BUILD_DIR}")
    cmake_path(ABSOLUTE_PATH PROGRAM_BUILD_DIR BASE_DIRECTORY "${CMAKE_SOURCE_DIR}" NORMALIZE)
endif()

foreach(_var IN ITEMS PROGRAM_OUTPUT_DIR PROGRAM_DATABASE_DIR MATERIAL_DATABASE_PATH PARTICLE_DATABASE_PATH)
    if(NOT IS_ABSOLUTE "${${_var}}")
        cmake_path(ABSOLUTE_PATH ${_var} BASE_DIRECTORY "${PROGRAM_BUILD_DIR}" NORMALIZE)
    endif()
endforeach()

file(MAKE_DIRECTORY "${PROGRAM_BUILD_DIR}")
file(MAKE_DIRECTORY "${PROGRAM_OUTPUT_DIR}")
file(MAKE_DIRECTORY "${PROGRAM_DATABASE_DIR}")
file(MAKE_DIRECTORY "${PROGRAM_BUILD_DIR}/Tools")

# -------------------------
# External dependencies
# -------------------------
# Used to convert readable json files to binaries
find_package(nlohmann_json REQUIRED) # JSON library

# The below will auto add the json library if it is not found

#find_package(nlohmann_json QUIET)
#if(NOT nlohmann_json_FOUND)
#    include(FetchContent)
#    FetchContent_Declare(
#            nlohmann_json
#            GIT_REPOSITORY https://github.com/nlohmann/json.git
#            GIT_TAG v3.11.3
#    )
#    FetchContent_MakeAvailable(nlohmann_json)
#endif()

# -------------------------
# Main executable
# -------------------------
add_executable(Simulation_program)

target_sources(Simulation_program PRIVATE
        app/main.cpp
        core/quantities/utilities/unit_utilities.cpp
        core/random/random_manager.cpp
        databases/base_database.cpp
        objects/object.cpp
        objects/object_manager.cpp
        objects/object-types/box.cpp
        objects/object-types/sphere.cpp
        particles/particle.cpp
        particles/particle_source.cpp
        particles/particle-types/atom.cpp
        particles/particle-types/photon.cpp
        physics/distributions.cpp
        physics/fields/field_solver.cpp
        physics/fields/field_boundary_handling.cpp
        physics/processes/interaction_utilities.cpp
        physics/processes/continuous/particle_continuous_interactions.cpp
        physics/processes/discrete/core/decay_utilities.cpp
        physics/processes/discrete/core/interaction_channels.cpp
        physics/processes/discrete/core/interaction_process_registry.cpp
        physics/processes/discrete/core/interaction_sampling.cpp
        physics/processes/discrete/interactions/photon_absorption.cpp
        physics/processes/discrete/interactions/spontaneous_emission.cpp
        simulation/data-collection/particle_collection.cpp
        simulation/geometry/boundary/boundary_interactions.cpp
        simulation/motion/particle_motion.cpp
        simulation/simulation_clock.cpp
        simulation/stepping/step_events.cpp
        simulation/stepping/step_manager.cpp
        simulation/stepping/step_utilities.cpp
)

# Include directories
target_include_directories(Simulation_program PRIVATE
        ${CMAKE_SOURCE_DIR}
        app
        config
        core
        core/linear-algebra
        core/physics
        core/quantities
        core/quantities/utilities
        core/random
        databases
        databases/material-data
        databases/particle-data
        databases/utilities
        objects
        objects/object-types
        objects/utilities
        particles
        particles/particle-types
        physics
        physics/fields
        physics/processes
        physics/processes/continuous
        physics/processes/discrete
        physics/processes/discrete/core
        physics/processes/discrete/interactions
        simulation
        simulation/data-collection
        simulation/geometry
        simulation/geometry/boundary
        simulation/motion
        simulation/stepping
)

set_target_properties(Simulation_program PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROGRAM_BUILD_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${PROGRAM_BUILD_DIR}"
        LIBRARY_OUTPUT_DIRECTORY "${PROGRAM_BUILD_DIR}"
)

target_link_libraries(Simulation_program PRIVATE nlohmann_json::nlohmann_json)

# -------------------------
# JSON -> BIN conversion + config.h auto-update
# -------------------------
# JSON files (configured in config/path_config.h)
set(JSON_FILES
        "${MATERIAL_DATABASE_JSON}"
        "${PARTICLE_DATABASE_JSON}"
)

# Directories
set(DB_BUILD_DIR "${PROGRAM_DATABASE_DIR}")
file(MAKE_DIRECTORY "${DB_BUILD_DIR}")

# json_to_bin helper
add_executable(json_to_bin EXCLUDE_FROM_ALL databases/json_to_bin.cpp)

target_sources(json_to_bin PRIVATE
        core/quantities/utilities/unit_utilities.cpp
        databases/base_database.cpp
)

target_include_directories(json_to_bin PRIVATE
        ${CMAKE_SOURCE_DIR}
        databases
        databases/utilities
        core
        core/linear-algebra
        core/quantities/utilities
)

target_link_libraries(json_to_bin PRIVATE nlohmann_json::nlohmann_json)

# Put the built json_to_bin binary in build/tools/
set_target_properties(json_to_bin PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROGRAM_BUILD_DIR}/Tools"
)

set(BIN_FILES "")

foreach(JSON_FILE IN LISTS JSON_FILES)
    # Absolute path to the source JSON (resolve relative to project root)
    set(JSON_FILE_ABS "${JSON_FILE}")
    if(NOT IS_ABSOLUTE "${JSON_FILE_ABS}")
        cmake_path(ABSOLUTE_PATH JSON_FILE_ABS BASE_DIRECTORY "${CMAKE_SOURCE_DIR}" NORMALIZE)
    endif()

    get_filename_component(FILE_NAME "${JSON_FILE_ABS}" NAME)
    string(REPLACE ".json" ".bin" BIN_NAME "${FILE_NAME}")
    # Use user-defined paths when available
    if(FILE_NAME STREQUAL "material_database.json")
        set(BIN_FILE "${MATERIAL_DATABASE_PATH}")
    elseif(FILE_NAME STREQUAL "particle_database.json")
        set(BIN_FILE "${PARTICLE_DATABASE_PATH}")
    else()
        set(BIN_FILE "${DB_BUILD_DIR}/${BIN_NAME}")
    endif()

    # Ensure target directory exists
    get_filename_component(BIN_DIR "${BIN_FILE}" DIRECTORY)
    file(MAKE_DIRECTORY "${BIN_DIR}")

    add_custom_command(
            OUTPUT "${BIN_FILE}"
            COMMAND $<TARGET_FILE:json_to_bin> "${JSON_FILE_ABS}" "${BIN_FILE}"
            DEPENDS "${JSON_FILE_ABS}" json_to_bin
            COMMENT "Converting ${JSON_FILE_ABS} → ${BIN_FILE}"
    )

    list(APPEND BIN_FILES "${BIN_FILE}")
endforeach()

# Custom target to group JSON conversion
add_custom_target(generate_databases ALL DEPENDS ${BIN_FILES})

add_dependencies(Simulation_program generate_databases)

# Include config in case it ends up in an odd directory or isn't included in the standard directory list
target_include_directories(Simulation_program PRIVATE "${CMAKE_SOURCE_DIR}/config")
