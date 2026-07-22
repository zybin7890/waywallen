if(NOT DEFINED GIT_EXECUTABLE OR NOT DEFINED SOURCE_DIR OR NOT DEFINED PATCH_FILE)
  message(FATAL_ERROR
          "ApplyPatch.cmake requires GIT_EXECUTABLE, SOURCE_DIR and PATCH_FILE")
endif()

execute_process(
  COMMAND "${GIT_EXECUTABLE}" apply --check --whitespace=nowarn "${PATCH_FILE}"
  WORKING_DIRECTORY "${SOURCE_DIR}"
  RESULT_VARIABLE _forward_result
  OUTPUT_VARIABLE _forward_output
  ERROR_VARIABLE _forward_error)

if(_forward_result EQUAL 0)
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" apply --whitespace=nowarn "${PATCH_FILE}"
    WORKING_DIRECTORY "${SOURCE_DIR}"
    RESULT_VARIABLE _apply_result
    OUTPUT_VARIABLE _apply_output
    ERROR_VARIABLE _apply_error)
  if(NOT _apply_result EQUAL 0)
    message(FATAL_ERROR
            "Failed to apply ${PATCH_FILE}:\n${_apply_output}${_apply_error}")
  endif()
  message(STATUS "Applied dependency patch: ${PATCH_FILE}")
  return()
endif()

execute_process(
  COMMAND "${GIT_EXECUTABLE}" apply --reverse --check --whitespace=nowarn
          "${PATCH_FILE}"
  WORKING_DIRECTORY "${SOURCE_DIR}"
  RESULT_VARIABLE _reverse_result
  OUTPUT_VARIABLE _reverse_output
  ERROR_VARIABLE _reverse_error)

if(_reverse_result EQUAL 0)
  message(STATUS "Dependency patch already applied: ${PATCH_FILE}")
  return()
endif()

message(
  FATAL_ERROR
    "Dependency patch does not apply cleanly: ${PATCH_FILE}\n"
    "Forward check:\n${_forward_output}${_forward_error}\n"
    "Reverse check:\n${_reverse_output}${_reverse_error}")
