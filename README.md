Libmetro is a C++ library for developing interesting metronome variants.

View the full API documentation, overview, tutorials, and examples [here](https://sevagh.github.io/libmetro). View the changelog [here](./CHANGELOG.md).

### Developer guide

Libmetro is built using cmake + Ninja (Ninja is a faster alternative to make). The provided top-level Makefile is for convenient access to the cmake targets. Libmetro builds and compiles using both clang and gcc, and targets the C++2a/C++20 standards.

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
        doxydev                 (blocking local http server)
```

The basics are build, test, and install. The tests ensure that the libsoundio defaults are cooperating on your system (initializing an output device with stereo output, etc.).

cpp-clean, build-ubsan, build-clang-tidy, and clang-analyze are different static analyzers and undefined behavior detection/correction tools. They're not mandatory (and in fact libmetro doesn't pass cleanly for most of them), but they provide important suggestions for making the code better.

clang-format is a code formatter tool and should be run before submitting a PR.

The doxygen target is for generating libmetro's API documentation and GitHub pages website. The doxydev target uses twisted to serve a local HTTP server for doc preview.
