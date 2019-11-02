Libmetro is a C++ library for developing interesting metronome variants.

Open a GitHub issue for any questions you may have.

### In a nutshell

Libmetro should empower users to create custom metronomes to practice a specific time signature, polyrhythm, or any other complex rhythmic feature. A typical metronome is only a click track with a bpm, and the user is expected to play time signatures etc. over the clicks, simply using the clicks to help keep time.

An example of a 4:3 polyrhythmic metronome that can help somebody learn how to play a 4:3 on a drum kit can be as follows:

1. Play accented 4/4 clicks for 10 measures with timbre 1 (drummer plays the 4/4 alongside, 10 times)
2. Play accented 3/4 clicks for 10 measures with timbre 2 (drummer plays the 3/4 alongside, 10 times)
3. Play both overlaid to create the 4:3 for 10 measures (drummer plays both pieces, 10 times)

This can be expressed easily with libmetro, and can be seen in [`examples/poly_4_3.cpp`](./examples/poly_4_3.cpp), or seen in action [here](LINK COMING). Ignoring argument parsing etc., the important parts of the code look like:

```c++
```

Full API documentation, high-level overview, and various sample clips and design docs are available at https://sevagh.github.io/libmetro

### Project structure

The libmetro sources are:

* [include](./include) - the single header file, `libmetro.h`, that you'll need to use libmetro
* [src](./src) - the source code of libmetro, split into private header files for modular design and readability
* [test](./test) - various unit tests
* [examples](./examples) - various examples (polyrhythms, compound/odd/simple time signatures, additive meter)

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
