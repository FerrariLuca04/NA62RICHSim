find_program(
    ROOT_EXECUTABLE
    root
    REQUIRED
)

# ---------------------------------------------------------
# Build test
# ---------------------------------------------------------

add_test(
    NAME primary_generator_proton

    COMMAND
        ${CMAKE_COMMAND}

        -DPROJECT_ROOT=${PROJECT_SOURCE_DIR}
        -DSIM_EXECUTABLE=$<TARGET_FILE:na62_rich_sim>
        -DROOT_EXECUTABLE=${ROOT_EXECUTABLE}

        -DMACRO=tests/primary_generator.mac
        -DCONFIG=tests/generator_proton_1GeV.conf

        -DOUTPUT_NAME=test_proton.root
        -DOUTPUT_FILE=${PROJECT_SOURCE_DIR}/output/test_proton.root

        "-DROOT_TEST_CALL=${PROJECT_SOURCE_DIR}/tests/root/test_primary_generator.C(\"output/test_proton.root\",2212)"

        -P
        ${PROJECT_SOURCE_DIR}/tests/scripts/run_test.cmake
)

add_test(
    NAME primary_generator_pion

    COMMAND
        ${CMAKE_COMMAND}

        -DPROJECT_ROOT=${PROJECT_SOURCE_DIR}
        -DSIM_EXECUTABLE=$<TARGET_FILE:na62_rich_sim>
        -DROOT_EXECUTABLE=${ROOT_EXECUTABLE}

        -DMACRO=tests/primary_generator.mac
        -DCONFIG=tests/generator_pion_1GeV.conf

        -DOUTPUT_NAME=test_pion.root
        -DOUTPUT_FILE=${PROJECT_SOURCE_DIR}/output/test_pion.root

        "-DROOT_TEST_CALL=${PROJECT_SOURCE_DIR}/tests/root/test_primary_generator.C(\"output/test_pion.root\",211)"

        -P
        ${PROJECT_SOURCE_DIR}/tests/scripts/run_test.cmake
)

add_test(
    NAME primary_generator_distribution

    COMMAND
        ${CMAKE_COMMAND}

        -DPROJECT_ROOT=${PROJECT_SOURCE_DIR}
        -DSIM_EXECUTABLE=$<TARGET_FILE:na62_rich_sim>
        -DROOT_EXECUTABLE=${ROOT_EXECUTABLE}

        -DMACRO=tests/distribution.mac
        -DCONFIG=tests/generator_distribution.conf

        -DOUTPUT_NAME=test_distribution.root
        -DOUTPUT_FILE=${PROJECT_SOURCE_DIR}/output/test_distribution.root

        "-DROOT_TEST_CALL=${PROJECT_SOURCE_DIR}/tests/root/test_primary_generator_distribution.C(\"output/test_distribution.root\",10000)"

        -P
        ${PROJECT_SOURCE_DIR}/tests/scripts/run_test.cmake
)

add_test(
    NAME cherenkov_process

    COMMAND
        ${CMAKE_COMMAND}

        -DPROJECT_ROOT=${PROJECT_SOURCE_DIR}
        -DSIM_EXECUTABLE=$<TARGET_FILE:na62_rich_sim>
        -DROOT_EXECUTABLE=${ROOT_EXECUTABLE}

        -DMACRO=tests/cherenkov.mac
        -DCONFIG=tests/generator_cherenkov.conf

        -DOUTPUT_NAME=test_cherenkov.root
        -DOUTPUT_FILE=${PROJECT_SOURCE_DIR}/output/test_cherenkov.root

        "-DROOT_TEST_CALL=${PROJECT_SOURCE_DIR}/tests/root/test_cherenkov.C(\"output/test_cherenkov.root\")"

        -P
        ${PROJECT_SOURCE_DIR}/tests/scripts/run_test.cmake
)

add_test(
    NAME cherenkov_energy

    COMMAND
        ${CMAKE_COMMAND}

        -DPROJECT_ROOT=${PROJECT_SOURCE_DIR}
        -DSIM_EXECUTABLE=$<TARGET_FILE:na62_rich_sim>
        -DROOT_EXECUTABLE=${ROOT_EXECUTABLE}

        -DMACRO=tests/distribution.mac
        -DCONFIG=tests/generator_energy_cherenkov.conf

        -DOUTPUT_NAME=test_energy_cherenkov.root
        -DOUTPUT_FILE=${PROJECT_SOURCE_DIR}/output/test_energy_cherenkov.root

        "-DROOT_TEST_CALL=${PROJECT_SOURCE_DIR}/tests/root/test_energy_cherenkov.C(\"output/test_energy_cherenkov.root\")"

        -P
        ${PROJECT_SOURCE_DIR}/tests/scripts/run_test.cmake
)

# ---------------------------------------------------------
# Label
# ---------------------------------------------------------

set_tests_properties(
    primary_generator_proton
    primary_generator_pion
    primary_generator_distribution

    PROPERTIES
    LABELS "primary_generator"
)

set_tests_properties(
    cherenkov_process
    cherenkov_energy
    
    PROPERTIES
    LABELS "cherenkov"
)

# ---------------------------------------------------------
# Working directory
# ---------------------------------------------------------

set_tests_properties(
    primary_generator_proton
    primary_generator_pion
    primary_generator_distribution
    cherenkov_process
    cherenkov_energy
    PROPERTIES
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
)