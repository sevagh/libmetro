#------------------------------------------------------------------------------#
# CMakeLists.txt
# based on https://github.com/RAttab/optics/blob/master/CMakeLists.txt
#------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 2.6)
project(libjungle)
set(VERSION 0.0.1)

enable_testing()

#------------------------------------------------------------------------------#
# BASICS
#------------------------------------------------------------------------------#

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

add_definitions("-Wall")
add_definitions("-Wextra")
add_definitions("-Wundef")
add_definitions("-Wunreachable-code")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++2a -pthread -D_GNU_SOURCE")

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

set(JUNGLE_SOURCES
    src/tempo.cpp
    src/audio/engine.cpp
    src/audio/tone.cpp
    src/audio/stream.cpp
    src/jungle.cpp)

set(JUNGLE_DEPS
    jungle_static
    ${SOUNDIO_LIBRARY})

add_library(jungle_static STATIC ${JUNGLE_SOURCES})
add_library(jungle SHARED ${JUNGLE_SOURCES})

target_link_libraries(jungle ${SOUNDIO_LIBRARY})

#------------------------------------------------------------------------------#
# BINS
#------------------------------------------------------------------------------#

function(jungle_example name)
	add_executable(${name}_example example/${name}_example.cpp)
	target_link_libraries(${name}_example ${JUNGLE_DEPS})
endfunction()

jungle_example(libjungle)

#------------------------------------------------------------------------------#
# INSTALL
#------------------------------------------------------------------------------#

install(TARGETS jungle jungle_static
    RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

install(FILES include/libjungle.h DESTINATION include)

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
	#add_library(jungle_tests STATIC test/bench.cpp test/test.cpp)
	#set(JUNGLE_TESTS_DEPS jungle_tests ${CMOCKA_LIBRARIES})
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
		add_executable(${name}_test test/${name}_test.cpp)
		target_link_libraries(${name}_test ${JUNGLE_TESTS_DEPS})

		add_test(${name}_test ${BIN_DIR}/${name}_test)
		set_tests_properties(${name}_test PROPERTIES LABELS "test")

		if(VALGRIND_EXEC)
		    add_test(${name}_valgrind ${VALGRIND_EXEC} ${VALGRIND_ARGS} ${BIN_DIR}/${name}_test)
		    set_tests_properties(${name}_valgrind PROPERTIES LABELS "valgrind")
		endif()
	endif()
endfunction()

#jungle_gtest(tempo)

#------------------------------------------------------------------------------#
# BENCHS
#------------------------------------------------------------------------------#

#function(jungle_cmocka_bench name)
#    if(CMOCKA_FOUND)
#        add_executable(${name}_bench test/${name}_bench.c)
#	target_link_libraries(${name}_bench jungle_tests ${JUNGLE_TESTS_DEPS})
#        set_property(TARGET ${name}_bench APPEND PROPERTY COMPILE_OPTIONS ${CMOCKA_CFLAGS})
#
#        add_test(${name}_bench ${BIN_DIR}/${name}_bench)
#        set_tests_properties(${name}_bench PROPERTIES LABELS "bench")
#    endif()
#endfunction()
#
#jungle_cmocka_bench(tempo)

#------------------------------------------------------------------------------#
# CLANG-TIDY
#------------------------------------------------------------------------------#

find_program(CLANG_TIDY_EXEC "clang-tidy")
if(CLANG_TIDY_EXEC)
    message("CLANG-TIDY: ${CLANG_TIDY_EXEC}")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_custom_target(clang-tidy
        COMMAND ${CLANG_TIDY_EXEC} -p ${CMAKE_BINARY_DIR} -header-filter='.*' src/*.cpp test/*.cpp example/*.cpp
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
else()
    message("CLANG-TIDY: DISABLED - install clang to enable")
endif()

#------------------------------------------------------------------------------#
# CLANG-FORMAT
#------------------------------------------------------------------------------#

find_program(CLANG_FORMAT_EXEC "clang-format")
if(CLANG_FORMAT_EXEC)
	message("CLANG-FORMAT: ${CLANG_FORMAT_EXEC}")
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	add_custom_target(clang-format
		COMMAND ${CLANG_FORMAT_EXEC} -style=Google -i src/*.cpp example/*.cpp include/*.h
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
else()
    message("CLANG-TIDY: DISABLED - install clang to enable")
endif()
