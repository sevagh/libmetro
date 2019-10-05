Version will be 0.0.1 for a while - very experimental codebase.

Dev dependencies:

* stk - https://github.com/thestk/stk
* libsoundio - https://github.com/andrewrk/libsoundio
* cppclean - https://github.com/myint/cppclean
* clang, clang-tools-extra
* valgrind for leak checks on the gtest tests

clang-tidy can be automatically run during the compile step:

```cmake
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
set_target_properties(
        jungle PROPERTIES
        CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
)
```

### Learning resources

These articles gave me some good tips on how I should approach real-time audio programming:

* http://www.rossbencina.com/code/real-time-audio-programming-101-time-waits-for-nothing
* http://atastypixel.com/blog/four-common-mistakes-in-audio-development/

I found the stk examples and libsoundio examples a good enough starting point to mold into working code.

### Notes

* my [fork of stk](https://github.com/sevagh/stk) has been compiled with `typedef float StkFloat` (instead of double) to match the floats I use in libjungle (enforced by libsoundio). RAWWAVE_PATH has been set to `./rawwaves/` and the directory has to be copied from stk to libjungle.
