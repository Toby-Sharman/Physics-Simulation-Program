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

# -------------------------
# External dependencies
# -------------------------
# Used to convert readable json files to binaries
# For a release version just binaries would be needed and this (including the json_to_bin stuff could be neglected)
find_package(nlohmann_json REQUIRED) # JSON library

# -------------------------
# Main executable
# -------------------------
add_executable(Simulation_program
        app/main.cpp
        objects/box.cpp
        objects/object.cpp
        objects/sphere.cpp
        objects/point.cpp
        particles/particle.cpp
        physics/fields/field_solver.cpp
        physics/fields/field_boundary_handling.cpp
        simulation/step.cpp
        simulation/data-collection/particle_collection.cpp
)

# Include directories
set(PROJECT_INCLUDE_DIR
        app
        config
        core
        core/maths
        core/maths/utilities
        databases
        databases/material-data
        databases/particle-data
        databases/utilities
        objects
        objects/utilities
        particles
        physics
        physics/fields
        simulation
        simulation/data-collection
)
target_include_directories(Simulation_program PRIVATE ${PROJECT_INCLUDE_DIR})

target_link_libraries(Simulation_program PRIVATE nlohmann_json::nlohmann_json)

# -------------------------
# json_to_bin helper
# -------------------------
add_executable(json_to_bin databases/json_to_bin.cpp)
set(JSON_TO_BIN_INCLUDE_DIR
        databases
        databases/utilities
        core/maths/utilities
)
target_include_directories(json_to_bin PRIVATE ${JSON_TO_BIN_INCLUDE_DIR})
target_link_libraries(json_to_bin PRIVATE nlohmann_json::nlohmann_json)

# -------------------------
# Generate binary databases and config.h
# -------------------------
# Handling the config directory is not strictly necessary as it should exist populated
# Possibly not included as part of a download of the whole program so included though

# Directories
set(DB_BUILD_DIR "${CMAKE_BINARY_DIR}/databases")
file(MAKE_DIRECTORY "${DB_BUILD_DIR}")

set(CONFIG_DIR "${CMAKE_SOURCE_DIR}/config")
file(MAKE_DIRECTORY "${CONFIG_DIR}")
set(CONFIG_FILE "${CONFIG_DIR}/config.h")

# Find all JSON files
file(GLOB_RECURSE JSON_FILES "${CMAKE_SOURCE_DIR}/databases/*.json")
set(BIN_FILES "")
set(DB_PATH_DEFINES "")

foreach(JSON_FILE IN LISTS JSON_FILES)
    # Get filename only
    get_filename_component(FILE_NAME "${JSON_FILE}" NAME)
    string(REPLACE ".json" ".bin" BIN_NAME "${FILE_NAME}")

    # Full output path
    set(BIN_FILE "${DB_BUILD_DIR}/${BIN_NAME}")

    # Custom command to convert JSON -> BIN
    add_custom_command(
            OUTPUT "${BIN_FILE}"
            COMMAND $<TARGET_FILE:json_to_bin> "${JSON_FILE}" "${BIN_FILE}"
            DEPENDS "${JSON_FILE}" json_to_bin
            COMMENT "Converting ${JSON_FILE} → ${BIN_FILE}"
    )

    list(APPEND BIN_FILES "${BIN_FILE}")

    # Fixed relative path for config.h
    set(REL_BIN_PATH "../build/databases/${BIN_NAME}")

    # Create #define
    get_filename_component(FILE_NAME_WE "${JSON_FILE}" NAME_WE)
    string(TOUPPER "${FILE_NAME_WE}" FILE_NAME_UPPER)
    set(DB_DEFINE_NAME "${FILE_NAME_UPPER}_DB_PATH")
    set(DB_PATH_DEFINES "${DB_PATH_DEFINES}#define ${DB_DEFINE_NAME} \"${REL_BIN_PATH}\"\n")
endforeach()

# Custom target to generate all binary databases
add_custom_target(generate_databases ALL DEPENDS ${BIN_FILES})

# Define the include guard name
set(CONFIG_INCLUDE_GUARD "PHYSICS_SIMULATION_PROGRAM_CONFIG_H")

# Write config.h with include guard - Could be better to write with \n but it the below is more legible
file(WRITE "${CONFIG_FILE}"
"#ifndef ${CONFIG_INCLUDE_GUARD}
#define ${CONFIG_INCLUDE_GUARD}

${DB_PATH_DEFINES}
#endif // ${CONFIG_INCLUDE_GUARD}"
)

# Include config in case it ends up in an odd directory or isn't included in the standard directory list
target_include_directories(Simulation_program PRIVATE "${CONFIG_DIR}")