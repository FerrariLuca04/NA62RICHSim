# ---------------------------------------------------------
# Cleanup old ROOT files
# ---------------------------------------------------------

file(REMOVE "${OUTPUT_FILE}")

# ---------------------------------------------------------
# Simulation
# ---------------------------------------------------------

execute_process(
    COMMAND
        "${SIM_EXECUTABLE}"
        --macro "${MACRO}"
        --config-generator "${CONFIG}"
        --output "${OUTPUT_NAME}"

    WORKING_DIRECTORY "${PROJECT_ROOT}"

    RESULT_VARIABLE simulation_result
)

if(NOT simulation_result EQUAL 0)
    message(FATAL_ERROR
        "Simulation failed with exit code ${simulation_result}"
    )
endif()

# ---------------------------------------------------------
# Validation
# ---------------------------------------------------------

execute_process(
    COMMAND
        "${ROOT_EXECUTABLE}"
        -l
        -b
        -q
        "${ROOT_TEST_CALL}"

    WORKING_DIRECTORY "${PROJECT_ROOT}"

    RESULT_VARIABLE validation_result
)

if(NOT validation_result EQUAL 0)
    message(FATAL_ERROR
        "Validation failed with exit code ${validation_result}"
    )
endif()

# ---------------------------------------------------------
# Cleanup old ROOT files
# ---------------------------------------------------------

file(REMOVE "${OUTPUT_FILE}")
