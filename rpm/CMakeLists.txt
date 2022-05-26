# SPDX-License-Identifier: Apache-2.0
# SPDX-FileCopyrightText: 2019-2022 Second State INC

# Add the git command here to support describing version without the root CMakeList.
find_program(GIT_CMD git)
execute_process(COMMAND
  ${GIT_CMD} describe --match "[0-9].[0-9]*" --tag
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE WASMEDGE_GIT_VERSION
  RESULT_VARIABLE GIT_VERSION_NOT_FOUND
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT GIT_VERSION_NOT_FOUND)
  string(REPLACE "-" "~" WASMEDGE_SRPM_VERSION ${WASMEDGE_GIT_VERSION})
endif()

configure_file(wasmedge.spec.in wasmedge.spec @ONLY)
