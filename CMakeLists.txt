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
        physics/distributions.cpp
        physics/fields/field_solver.cpp
        physics/fields/field_boundary_handling.cpp
        physics/interactions/continuous/particle_continuous_interactions.cpp
        physics/interactions/discrete/boundary_interactions.cpp
        physics/interactions/interaction_utilities.cpp
        simulation/step.cpp
        simulation/geometry/boundary/boundary_interactions.cpp
        simulation/data-collection/particle_collection.cpp
)

# Include directories
target_include_directories(Simulation_program PRIVATE
        ${CMAKE_SOURCE_DIR}
        app
        config
        core
        core/linear-algebra
        core/physics
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
        physics
        physics/interactions
        physics/interactions/continuous
        physics/interactions/discrete
        physics/fields
        simulation
        simulation/geometry
        simulation/data-collection
)

target_link_libraries(Simulation_program PRIVATE nlohmann_json::nlohmann_json)

# -------------------------
# JSON -> BIN conversion + config.h auto-update
# -------------------------
# JSON files
set(JSON_FILES
        databases/material-data/material_database.json
        databases/particle-data/particle_database.json
)

# Directories
set(DB_BUILD_DIR "${CMAKE_BINARY_DIR}/Databases")
file(MAKE_DIRECTORY "${DB_BUILD_DIR}")

set(CONFIG_DIR "${CMAKE_SOURCE_DIR}/config")
file(MAKE_DIRECTORY "${CONFIG_DIR}")
set(CONFIG_FILE "${CONFIG_DIR}/config.h")
set(CONFIG_INCLUDE_GUARD "PHYSICS_SIMULATION_PROGRAM_CONFIG_H")

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
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Tools"
)

set(BIN_FILES "")
set(DB_PATH_DEFINES "")

foreach(JSON_FILE IN LISTS JSON_FILES)
    # Absolute path to the source JSON
    set(JSON_FILE_ABS "${CMAKE_SOURCE_DIR}/${JSON_FILE}")

    get_filename_component(FILE_NAME "${JSON_FILE}" NAME)
    string(REPLACE ".json" ".bin" BIN_NAME "${FILE_NAME}")
    set(BIN_FILE "${DB_BUILD_DIR}/${BIN_NAME}")

    add_custom_command(
            OUTPUT "${BIN_FILE}"
            COMMAND $<TARGET_FILE:json_to_bin> "${JSON_FILE_ABS}" "${BIN_FILE}"
            DEPENDS "${JSON_FILE_ABS}" json_to_bin
            COMMENT "Converting ${JSON_FILE_ABS} → ${BIN_FILE}"
    )

    list(APPEND BIN_FILES "${BIN_FILE}")

    # For config.h
    file(RELATIVE_PATH REL_BIN_PATH "${CONFIG_DIR}" "${BIN_FILE}")
    get_filename_component(FILE_NAME_WE "${JSON_FILE}" NAME_WE)
    string(TOUPPER "${FILE_NAME_WE}" FILE_NAME_UPPER)
    set(DB_DEFINE_NAME "${FILE_NAME_UPPER}_PATH")
    set(DB_PATH_DEFINES "${DB_PATH_DEFINES}#define ${DB_DEFINE_NAME} \"${REL_BIN_PATH}\"\n")
endforeach()

# Write config.h with include guard - Could be better to write with \n but it the below is more legible
file(WRITE "${CONFIG_FILE}" "#ifndef ${CONFIG_INCLUDE_GUARD}\n#define ${CONFIG_INCLUDE_GUARD}\n\n${DB_PATH_DEFINES}\n#endif // ${CONFIG_INCLUDE_GUARD}")

# Custom target to group JSON conversion
add_custom_target(generate_databases ALL DEPENDS ${BIN_FILES})

add_dependencies(Simulation_program generate_databases)

# Include config in case it ends up in an odd directory or isn't included in the standard directory list
target_include_directories(Simulation_program PRIVATE "${CONFIG_DIR}")