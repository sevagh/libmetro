#------------------------------------------------------------------------------#
# CMakeLists.txt
# based on https://github.com/RAttab/optics/blob/master/CMakeLists.txt
#------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.14)
project(libjungle)
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
find_package(Stk REQUIRED)

#------------------------------------------------------------------------------#
# LIBS
#------------------------------------------------------------------------------#

include_directories("${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include" ${GTEST_INCLUDE_DIRS} ${SOUNDIO_INCLUDE_DIR} ${STK_INCLUDE_DIR})
link_directories(${GTEST_LIBRARY_DIRS})

set(JUNGLE_SOURCES
    src/tempo.cpp
    src/event.cpp
    src/audio_engine.cpp
    src/audio_outstream.cpp
    src/metronome.cpp
    src/timbre.cpp)

set(JUNGLE_DEPS
    jungle_static
    ${SOUNDIO_LIBRARY}
    ${STK_LIBRARY})

add_library(jungle_static STATIC ${JUNGLE_SOURCES})
add_library(jungle SHARED ${JUNGLE_SOURCES})

target_link_libraries(jungle ${SOUNDIO_LIBRARY} ${STK_LIBRARY})

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
#	jungle PROPERTIES
#	CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
#)

#------------------------------------------------------------------------------#
# BINS
#------------------------------------------------------------------------------#

function(jungle_program name)
	add_executable(program_${name} programs/${name}.cpp)
	target_link_libraries(program_${name} ${JUNGLE_DEPS})
endfunction()

jungle_program(drum_track_4_4)
jungle_program(tap_tempo)
jungle_program(switch_metronome)

#------------------------------------------------------------------------------#
# INSTALL
#------------------------------------------------------------------------------#

install(TARGETS jungle jungle_static
    RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

install(FILES include/libjungle.h include/libjungle_synthesis.h DESTINATION include/libjungle)

set(pc_version ${VERSION})
set(pc_prefix ${CMAKE_INSTALL_PREFIX})

function(jungle_pc name)
    configure_file(src/${name}.pc.in ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${name}.pc @ONLY)
    install(FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${name}.pc DESTINATION lib/pkgconfig)
endfunction()

jungle_pc(jungle)
jungle_pc(jungle_static)

#------------------------------------------------------------------------------#
# TESTS
#------------------------------------------------------------------------------#

if(GTEST_FOUND)
	set(JUNGLE_TESTS_DEPS ${JUNGLE_DEPS} gtest gtest_main pthread)
endif()

find_program(VALGRIND_EXEC "valgrind")
if(VALGRIND_EXEC)
    message("VALGRIND: ${VALGRIND_EXEC}")
    set(VALGRIND_ARGS --leak-check=full --track-origins=yes)
else()
    message("VALGRIND: DISABLED - install valgrind to enable")
endif()

function(jungle_gtest name)
	if(GTEST_FOUND)
		add_executable(test_${name} test/${name}.cpp)
		target_link_libraries(test_${name} ${JUNGLE_TESTS_DEPS})

		add_test(test_${name} ${BIN_DIR}/test_${name})
		set_tests_properties(test_${name} PROPERTIES LABELS "test")

		if(VALGRIND_EXEC)
		    add_test(${name}_valgrind ${VALGRIND_EXEC} ${VALGRIND_ARGS} ${BIN_DIR}/test_${name})
		    set_tests_properties(${name}_valgrind PROPERTIES LABELS "valgrind")
		endif()
	endif()
endfunction()

jungle_gtest(audio_engine)
jungle_gtest(clock_accuracy)

#------------------------------------------------------------------------------#
# CLANG_FORMAT, CPPCLEAN
#------------------------------------------------------------------------------#

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
