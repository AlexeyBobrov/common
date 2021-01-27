if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" )
  if (NOT LLVM_PROFDATA OR NOT LLVM_COV)
    message(WARNING "Not found llvm coverage tools")
  else()
    
    add_custom_target(gcov
      COMMAND ${LLVM_PROFDATA} merge -sparse default.profraw -o ${ProjectName}_test.profdata
      COMMAND ${LLVM_COV} show ${ProjectName}_test -instr-profile=${ProjectName}_test.profdata
      COMMAND ${LLVM_COV} report ${ProjectName}_test -instr-profile=${ProjectName}_test.profdata
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

  endif()
elseif ( "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" )
  find_program(GCOVR NAME gcovr PATHS /usr/bin/ /opt/local/bin)
  if (NOT GCOVR)
    message(FATAL_ERROR "Not found gcovr")
  endif()

  message(DEBUG "Gcovr program ${GCOVR}")

  add_custom_target(gcov
    COMMAND ${GCOVR} --gcov-executable=${GCOV} -r ${CMAKE_CURRENT_SOURCE_DIR}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

endif()