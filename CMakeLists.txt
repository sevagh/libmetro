#------------------------------------------------------------------------------#
# CMakeLists.txt
# based on https://github.com/RAttab/optics/blob/master/CMakeLists.txt
#------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.14)
project(libmetro)
set(VERSION 0.0.1)

enable_testing()

#------------------------------------------------------------------------------#
# BASICS
#------------------------------------------------------------------------------#

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 20)

set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

add_definitions("-Wall")
add_definitions("-Wextra")
add_definitions("-Wundef")
add_definitions("-Wunreachable-code")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -pthread")

#------------------------------------------------------------------------------#
# DEPS
#------------------------------------------------------------------------------#

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(FindPkgConfig)

find_package(GTest)
if(GTest_FOUND)
	message("GTest: ON")
else()
	message("GTest: OFF")
endif()

find_package(SoundIo REQUIRED)

#------------------------------------------------------------------------------#
# LIBS
#------------------------------------------------------------------------------#

include_directories("${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include" ${GTEST_INCLUDE_DIRS} ${SOUNDIO_INCLUDE_DIR})
link_directories(${GTEST_LIBRARY_DIRS})

set(METRO_SOURCES
    src/audioengine.cpp
    src/outstream.cpp
    src/metronome.cpp
    src/timbregen.cpp)

set(METRO_DEPS
    metro
    ${SOUNDIO_LIBRARY})

add_library(metro SHARED ${METRO_SOURCES})
target_link_libraries(metro ${SOUNDIO_LIBRARY})

#------------------------------------------------------------------------------#
# CLANG-TIDY
#------------------------------------------------------------------------------#

find_program(
  CLANG_TIDY_EXE
  NAMES "clang-tidy"
  DOC "Path to clang-tidy executable"
  )
if(NOT CLANG_TIDY_EXE)
  message(STATUS "clang-tidy not found.")
else()
  message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
  set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-checks=*,-clang-analyzer-alpha.*")
endif()
#uncomment this
#set_target_properties(
#	metro PROPERTIES
#	CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
#)

#------------------------------------------------------------------------------#
# BINS
#------------------------------------------------------------------------------#

function(metro_program name)
	add_executable(program_${name} programs/${name}.cpp)
	target_link_libraries(program_${name} ${METRO_DEPS})
endfunction()

metro_program(click_track)
metro_program(tap_tempo)

#------------------------------------------------------------------------------#
# INSTALL
#------------------------------------------------------------------------------#

install(TARGETS metro
    RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

install(FILES include/libmetro.h include/libmetro_synthesis.h DESTINATION include/libmetro)

#------------------------------------------------------------------------------#
# TESTS
#------------------------------------------------------------------------------#

if(GTEST_FOUND)
	set(METRO_TESTS_DEPS ${METRO_DEPS} gtest gtest_main pthread)
endif()

find_program(VALGRIND_EXEC "valgrind")
if(VALGRIND_EXEC)
    message("VALGRIND: ${VALGRIND_EXEC}")
    set(VALGRIND_ARGS --leak-check=full --track-origins=yes)
else()
    message("VALGRIND: DISABLED - install valgrind to enable")
endif()

function(metro_gtest name)
	if(GTEST_FOUND)
		add_executable(test_${name} test/${name}.cpp)
		target_link_libraries(test_${name} ${METRO_TESTS_DEPS})
		target_compile_definitions(test_${name} PUBLIC UNIT_TESTS )

		add_test(test_${name} ${BIN_DIR}/test_${name})
		set_tests_properties(test_${name} PROPERTIES LABELS "test")

		if(VALGRIND_EXEC)
		    add_test(${name}_valgrind ${VALGRIND_EXEC} ${VALGRIND_ARGS} ${BIN_DIR}/test_${name})
		    set_tests_properties(${name}_valgrind PROPERTIES LABELS "valgrind")
		endif()
	endif()
endfunction()

function(metro_gtest_long name)
	if(GTEST_FOUND)
		add_executable(test_long_${name} test/${name}.cpp)
		target_link_libraries(test_long_${name} ${METRO_TESTS_DEPS})
	endif()
endfunction()

metro_gtest_long(clock_accuracy)
metro_gtest(unit)

#------------------------------------------------------------------------------#
# CLANG_FORMAT, CPPCLEAN
#------------------------------------------------------------------------------#

file(GLOB_RECURSE
     ALL_CXX_SOURCE_FILES
     src/*.cpp src/*.h programs/*.cpp include/*.h test/*.cpp
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
