#------------------------------------------------------------------------------#
# CMakeLists.txt
# based on https://github.com/RAttab/optics/blob/master/CMakeLists.txt
#------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 2.6)
project(rhythm_n_beats)
set(VERSION 0.0.1)

enable_testing()

#------------------------------------------------------------------------------#
# BASICS
#------------------------------------------------------------------------------#

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(BIN_DIR ${PROJECT_SOURCE_DIR}/bin)

add_definitions("-pipe -g -O3 -Werror -march=native")
add_definitions("-Wall")
add_definitions("-Wextra")
add_definitions("-Wundef")
add_definitions("-Wcast-align")
add_definitions("-Wwrite-strings")
add_definitions("-Wunreachable-code")
add_definitions("-Wformat=2")
add_definitions("-Wswitch-enum")
add_definitions("-Wswitch-default")
add_definitions("-Winit-self")
add_definitions("-Wno-strict-aliasing")

# clang rejects standard compliant `{0}` struct initializers. It's a known bug.
add_definitions("-Wno-missing-field-initializers")

# Our log utils needs the format to be passed through a function and I really
# don't see the issue with that so...
add_definitions("-Wno-format-nonliteral")

# GCC 9 consider -Waddress-of-packed-member an error by default, so we ignore it.
if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 9)
    add_definitions("-Wno-error=address-of-packed-member")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -std=gnu11 -pthread -D_GNU_SOURCE")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DRNB_VERSION=\"${VERSION}\"")


#------------------------------------------------------------------------------#
# DEPS
#------------------------------------------------------------------------------#

include(FindPkgConfig)

pkg_check_modules(GTEST "gtest")
if(GTEST_FOUND)
	message("GTEST: ON")
else()
	message("GTEST: OFF")
endif()

#------------------------------------------------------------------------------#
# LIBS
#------------------------------------------------------------------------------#

include_directories("${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include" ${GTEST_INCLUDE_DIRS})
link_directories(${GTEST_LIBRARY_DIRS})

set(RNB_SOURCES
    src/tempo.cpp)

set(RNB_DEPS
    rnb_static
    rt)

add_library(rnb_static STATIC ${RNB_SOURCES})
add_library(rnb SHARED ${RNB_SOURCES})
target_link_libraries(rnb rt)

#------------------------------------------------------------------------------#
# BINS
#------------------------------------------------------------------------------#

function(rnb_example name)
	add_executable(${name}_example example/${name}_example.cpp)
	target_link_libraries(${name}_example ${RNB_DEPS})
endfunction()

rnb_example(tempo)

#------------------------------------------------------------------------------#
# INSTALL
#------------------------------------------------------------------------------#

install(TARGETS rnb rnb_static
    RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

install(FILES include/rhythm_n_beats.h DESTINATION include)

set(pc_version ${VERSION})
set(pc_prefix ${CMAKE_INSTALL_PREFIX})

function(rnb_pc name)
    configure_file(src/${name}.pc.in ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${name}.pc @ONLY)
    install(FILES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${name}.pc DESTINATION lib/pkgconfig)
endfunction()

rnb_pc(rnb)
rnb_pc(rnb_static)

#------------------------------------------------------------------------------#
# TESTS
#------------------------------------------------------------------------------#

if(GTEST_FOUND)
	#add_library(rnb_tests STATIC test/bench.cpp test/test.cpp)
	#set(RNB_TESTS_DEPS rnb_tests ${CMOCKA_LIBRARIES})
	set(RNB_TESTS_DEPS ${RNB_DEPS} gtest gtest_main pthread)
endif()

find_program(VALGRIND_EXEC "valgrind")
if(VALGRIND_EXEC)
    message("VALGRIND: ${VALGRIND_EXEC}")
    set(VALGRIND_ARGS --leak-check=full --track-origins=yes)
else()
    message("VALGRIND: DISABLED - install valgrind to enable")
endif()

function(rnb_gtest name)
	if(GTEST_FOUND)
		add_executable(${name}_test test/${name}_test.cpp)
		target_link_libraries(${name}_test ${RNB_TESTS_DEPS})

		add_test(${name}_test ${BIN_DIR}/${name}_test)
		set_tests_properties(${name}_test PROPERTIES LABELS "test")

		if(VALGRIND_EXEC)
		    add_test(${name}_valgrind ${VALGRIND_EXEC} ${VALGRIND_ARGS} ${BIN_DIR}/${name}_test)
		    set_tests_properties(${name}_valgrind PROPERTIES LABELS "valgrind")
		endif()
	endif()
endfunction()

rnb_gtest(tempo)

#------------------------------------------------------------------------------#
# BENCHS
#------------------------------------------------------------------------------#

#function(rnb_cmocka_bench name)
#    if(CMOCKA_FOUND)
#        add_executable(${name}_bench test/${name}_bench.c)
#	target_link_libraries(${name}_bench rnb_tests ${RNB_TESTS_DEPS})
#        set_property(TARGET ${name}_bench APPEND PROPERTY COMPILE_OPTIONS ${CMOCKA_CFLAGS})
#
#        add_test(${name}_bench ${BIN_DIR}/${name}_bench)
#        set_tests_properties(${name}_bench PROPERTIES LABELS "bench")
#    endif()
#endfunction()
#
#rnb_cmocka_bench(tempo)

#------------------------------------------------------------------------------#
# UBSAN
#------------------------------------------------------------------------------#

option(ENABLE_UBSAN
    "enable undefined behaviour sanitizer (affects performance)" OFF)
if(ENABLE_UBSAN)
    message("UBSAN: ENABLED")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=undefined")
else()
    message("UBSAN: DISABLED")
endif()

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
		COMMAND ${CLANG_FORMAT_EXEC} -i src/*.cpp test/*.cpp example/*.cpp include/*.h
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
else()
    message("CLANG-TIDY: DISABLED - install clang to enable")
endif()
