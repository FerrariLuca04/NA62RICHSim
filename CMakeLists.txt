cmake_minimum_required(VERSION 3.16)

project(
    NA62RICHSim
    VERSION 0.3.3
    DESCRIPTION "Educational Geant4 simulation of the NA62 RICH detector"
    LANGUAGES CXX
)

find_package(
    Geant4
    REQUIRED
    ui_all
    vis_all
)

find_package(
    ROOT REQUIRED
    COMPONENTS
        Core
        RIO
        Tree
)

add_executable(
    na62_rich_sim
    app/main.cc
)

add_subdirectory(src)

target_include_directories(
    na62_rich_sim
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(
    na62_rich_sim
    PRIVATE
        cxx_std_17
)

target_link_libraries(
    na62_rich_sim
    PRIVATE
        ${Geant4_LIBRARIES}

        ROOT::Core
        ROOT::RIO
        ROOT::Tree
)

set_target_properties(
    na62_rich_sim
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin"
)