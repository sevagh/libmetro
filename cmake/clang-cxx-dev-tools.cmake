file(GLOB_RECURSE
     ALL_CXX_SOURCE_FILES
     src/*.cpp programs/*.cpp include/*.h test/*.cpp
     )

# Adding clang-format target if executable is found
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
  add_custom_target(
    clang-format
    COMMAND clang-format
    -i
    -style=file
    ${ALL_CXX_SOURCE_FILES}
    )
endif()

# Adding cppclean target if executable is found
find_program(CPP_CLEAN "cppclean")
if(CPP_CLEAN)
  add_custom_target(
    cpp-clean
    COMMAND cppclean
    --include-path ../include/
    ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif()
