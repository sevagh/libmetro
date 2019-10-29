Libmetro is a C++ library for developing interesting metronome variants.

Open a GitHub issue for any questions you may have.

### In a nutshell

A traditional African polyrhythm recreated in [67 lines of C++](./demo/butour_ngale.cpp):

https://sevagh.github.io/libmetro/md__home_sevagh_repos_libmetro_doxygen_3_musicdemos.html

### Project structure

A description of the project, API docs, and various sample clips are available at: https://sevagh.github.io/libmetro

The libmetro sources are:

* [include](./include) - the single header file, `libmetro.h`, that you'll need to use libmetro
* [src](./src) - the source code of libmetro, split into private header files for modular design and readability
* [test](./test) - various unit tests

Programs built with libmetro are:

* [example](./example) - very basic examples, the gentlest introduction to libmetro
* [demo](./demo) - demo programs implementing specific time signatures from real songs and live music
* [programs](./programs) - more complex programs that implement a variety of simple, odd, compound and polyrhythmic time signatures

### Developer guide

Libmetro is built using cmake + Ninja (Ninja is a faster alternative to make). The provided top-level Makefile is for convenient access to the cmake targets. Libmetro builds and compiles using both clang and gcc, and targets the C++2a/C++20 standards (thus requiring modern clang and gcc).

The following `dnf` command installs all the dependencies on Fedora 30:

```
$ dnf install make \
                cmake \
                clang \
                gcc \
                clang-tools-extra \
                clang-analyzer \
                libubsan \
                libasan \
                ninja-build \
                valgrind \
                gtest-devel \
                doxygen
```

Additionally, you need to clone, build, and install the following projects:

* https://github.com/andrewrk/libsoundio - MIT license
* https://github.com/thestk/stk - MIT-compatible license

Make targets (that are mostly self-explanatory):

```
$ make
libmetro targets:
        clean
        build
        build-ubsan             (needs a clean)
        build-clang-tidy        (needs a clean)
        test
        install
        cpp-clean
        clang-analyze
        clang-format
        doxygen
        docdev                  (blocking local http server)
        readmedev               (blocking local http server)
```

The basics are build, test, and install. The tests ensure that the libsoundio defaults are cooperating on your system (initializing an output device with stereo output, etc.).

cpp-clean, build-ubsan, build-clang-tidy, and clang-analyze are different static analyzers and undefined behavior detection/correction tools. They're not mandatory (and in fact libmetro doesn't pass cleanly for most of them), but they provide important suggestions for making the code better.

clang-format is a code formatter tool and should be run before submitting a PR.

Documentation tools are even more optional - I use some Python tools for convenient local testing and rendering of docs:

```
$ pip3.7 install --user grip \
                        twisted
```

The target doxygen regenerates docs using Doxygen. The targets docdev and readmedev serve documentation on local webservers.
