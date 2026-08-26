find_program(
    ROOT_EXECUTABLE
    root
    REQUIRED
)

# ---------------------------------------------------------
# Cleanup old ROOT files
# ---------------------------------------------------------

add_test(
    NAME primary_generator_proton_cleanup
    COMMAND
        ${CMAKE_COMMAND}
        -E rm -f
        "${PROJECT_SOURCE_DIR}/output/test_proton.root"
)

add_test(
    NAME primary_generator_pion_cleanup
    COMMAND
        ${CMAKE_COMMAND}
        -E rm -f
        "${PROJECT_SOURCE_DIR}/output/test_pion.root"
)

add_test(
    NAME primary_generator_distribution_cleanup
    COMMAND
        ${CMAKE_COMMAND}
        -E rm -f
        "${PROJECT_SOURCE_DIR}/output/test_distribution.root"
)

# ---------------------------------------------------------
# Simulation
# ---------------------------------------------------------

add_test(
    NAME primary_generator_proton_simulation
    COMMAND
        $<TARGET_FILE:na62_rich_sim>
        --macro tests/primary_generator.mac
        --config-generator tests/generator_proton_1GeV.conf
        --output test_proton.root
)

add_test(
    NAME primary_generator_pion_simulation
    COMMAND
        $<TARGET_FILE:na62_rich_sim>
        --macro tests/primary_generator.mac
        --config-generator tests/generator_pion_1GeV.conf
        --output test_pion.root
)

add_test(
    NAME primary_generator_distribution_simulation
    COMMAND
        $<TARGET_FILE:na62_rich_sim>
        --macro tests/distribution.mac
        --config-generator tests/generator_distribution.conf
        --output test_distribution.root
)

# ---------------------------------------------------------
# Validation
# ---------------------------------------------------------

add_test(
    NAME primary_generator_proton_validation
    COMMAND
        ${ROOT_EXECUTABLE}
        -l
        -b
        -q
        "${PROJECT_SOURCE_DIR}/tests/root/test_primary_generator.C(\"output/test_proton.root\",2212,1.0,1.000000001)"
)

add_test(
    NAME primary_generator_pion_validation
    COMMAND
        ${ROOT_EXECUTABLE}
        -l
        -b
        -q
        "${PROJECT_SOURCE_DIR}/tests/root/test_primary_generator.C(\"output/test_pion.root\",211,1.0,1.000000001)"
)

add_test(
    NAME primary_generator_distribution_validation
    COMMAND
        ${ROOT_EXECUTABLE}
        -l
        -b
        -q
        "${PROJECT_SOURCE_DIR}/tests/root/test_primary_generator_distribution.C(\"output/test_distribution.root\",20.0,50.0,330,1410,15,122,10000)"
)

# ---------------------------------------------------------
# Dependencies
# ---------------------------------------------------------

set_tests_properties(
    primary_generator_proton_simulation
    PROPERTIES
    DEPENDS primary_generator_proton_cleanup
)

set_tests_properties(
    primary_generator_pion_simulation
    PROPERTIES
    DEPENDS primary_generator_pion_cleanup
)

set_tests_properties(
    primary_generator_distribution_simulation
    PROPERTIES
    DEPENDS primary_generator_distribution_cleanup
)

set_tests_properties(
    primary_generator_proton_validation
    PROPERTIES
    DEPENDS primary_generator_proton_simulation
)

set_tests_properties(
    primary_generator_pion_validation
    PROPERTIES
    DEPENDS primary_generator_pion_simulation
)

set_tests_properties(
    primary_generator_distribution_validation
    PROPERTIES
    DEPENDS primary_generator_distribution_simulation
)

# ---------------------------------------------------------
# Working directory
# ---------------------------------------------------------

set_tests_properties(
    primary_generator_proton_cleanup
    primary_generator_pion_cleanup
    primary_generator_distribution_cleanup
    primary_generator_proton_simulation
    primary_generator_pion_simulation
    primary_generator_distribution_simulation
    primary_generator_proton_validation
    primary_generator_pion_validation
    primary_generator_distribution_validation
    PROPERTIES
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)