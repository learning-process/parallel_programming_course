if(NOT DEFINED PROJECT_SOURCE_DIR)
  message(FATAL_ERROR "PROJECT_SOURCE_DIR must be defined")
endif()

find_package(Git REQUIRED)

execute_process(
  COMMAND "${GIT_EXECUTABLE}" status --porcelain -- docs/locale
  WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
  OUTPUT_VARIABLE docs_i18n_status
  RESULT_VARIABLE docs_i18n_status_result
  ERROR_VARIABLE docs_i18n_status_error)

if(NOT docs_i18n_status_result EQUAL 0)
  message(
    FATAL_ERROR
      "Failed to inspect docs/locale status:\n${docs_i18n_status_error}")
endif()

if(docs_i18n_status)
  execute_process(
    COMMAND "${GIT_EXECUTABLE}" diff --stat -- docs/locale
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    OUTPUT_VARIABLE docs_i18n_diff_stat
    ERROR_VARIABLE docs_i18n_diff_stat_error)

  message(STATUS "Stale Sphinx translation files:")
  message(STATUS "${docs_i18n_status}")

  if(docs_i18n_diff_stat)
    message(STATUS "Diff stat after regenerating Sphinx translations:")
    message(STATUS "${docs_i18n_diff_stat}")
  endif()

  message(
    FATAL_ERROR
      "Sphinx translations are out of date. Run `cmake --build <build-dir> -t docs_update` and commit the docs/locale changes."
  )
endif()

message(STATUS "Sphinx translations are up to date")
