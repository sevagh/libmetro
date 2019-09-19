#thanks https://www.labri.fr/perso/fleury/posts/programming/using-clang-tidy-and-clang-format.html

file(GLOB_RECURSE
     ALL_CXX_SOURCE_FILES
     *.cpp *.h
     )

# Adding clang-format target if executable is found
find_program(CLANG_FORMAT "clang-format")
if(CLANG_FORMAT)
  add_custom_target(
    clang-format
    COMMAND /usr/bin/clang-format
    -i
    -style=file
    ${ALL_CXX_SOURCE_FILES}
    )
endif()

# Adding clang-tidy target if executable is found
find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
  add_custom_target(
    clang-tidy
    COMMAND /usr/bin/clang-tidy
    ${ALL_CXX_SOURCE_FILES}
    -config=''
    --
    -std=c++2a
    ${INCLUDE_DIRECTORIES}
    )
endif()
