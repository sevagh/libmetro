Libmetro is a C++ library for developing interesting metronome variants.

**link to github-pages for complete API docs and real demos are coming soon**

Feel free to open a GitHub issue for any questions you may have.

### Synopsis

The goal of libmetro is to simplify the development of specialized metronomes, e.g. simple, odd, compound, additive, and polyrhythmic click/drum tracks for practicing. Many of the examples and programs are adapted from the excellent [Bounce Metronome](https://bouncemetronome.com/audio/downloadable-audio-clips/audio-clips-time-signatures-additive-rhythms-and-polyrhythm) website.

The provided classes are Metronome, Note, and Measure. The bpm of the metronome should **always be given as the bpm of the quarter note**.

A Note is a convenience wrapper around a vector of floats representing a single sound - choices of timbre include `Timbre::{Sine, Drum}`. In the case of Drum, the frequency is the [general MIDI drum note](https://en.wikipedia.org/wiki/General_MIDI#Percussion), e.g. 56.0 for cowbell.

A Measure is a convenience wrapper around a vector of Notes representing a measure. Measures are added to a metronome with their note length, represented with the enum `NoteLength::{Half, Quarter, QuarterTriplet, Eighth, EighthTriplet, Sixteenth}`.

### Basic usage

Here's a simple example of an accented 4/4 beat with sine waves:

```c++
#include "libmetro.h"

int bpm = 100;
auto metronome = metro::Metronome(bpm);

auto downbeat = metro::Note(metro::Timbre::Sine, 540.0, 100.0);
auto weakbeat = metro::Note(metro::Timbre::Sine, 350.0, 50.0);
auto mediumbeat = metro::Note(metro::Timbre::Sine, 440.0, 65.0);

metro::Measure accented_44(4);
accented_44[0] = downbeat;
accented_44[1] = weakbeat;
accented_44[2] = mediumbeat;
accented_44[3] = weakbeat;

metronome.add_measure(metro::NoteLength::Quarter, accented_44);
metronome.start_and_loop();
```

Listen: [accented 4/4, 100bpm](./.github/accented_4_4_demo.wav)

Here's a more complex example of a 3:2 polyrhythm/cross-rhythm. The triples are played with snares (+ hihat for the downbeat), and the duples are played with cowbell (+ tambourine for the downbeat):

```c++
#include "libmetro.h"

int bpm = 100;
auto metronome = metro::Metronome(bpm);

auto poly1 = metro::Measure(2);
poly1[0] = metro::Note(metro::Timbre::Drum, 54.0, 100.0)
                   + metro::Note(metro::Timbre::Drum, 56.0, 100.0);
poly2[1] = metro::Note(metro::Timbre::Drum, 56.0, 50.0);

metronome.add_measure(metro::NoteLength::Quarter, poly1);

auto poly2 = metro::Measure(3);
poly2[0] = metro::Note(metro::Timbre::Drum, 38.0, 100.0)
                     + metro::Note(metro::Timbre::Drum, 42.0, 100.0);
poly2[1] = metro::Note(metro::Timbre::Drum, 38.0, 50.0);
poly2[2] = metro::Note(metro::Timbre::Drum, 38.0, 50.0);

metronome.add_measure(
    metro::NoteLength::QuarterTriplet, poly2); // note the QuarterTriplet note length

metronome.start_and_loop();
```

Listen: [3:2 polyrhythm, 100bpm](./.github/poly_3_2_demo.wav)

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
                ninja-build \
                valgrind \
                gtest-devel \
                doxygen
```

Additionally, you need to clone, build, and install the following projects:

* https://github.com/andrewrk/libsoundio - MIT license
* https://github.com/thestk/stk - MIT-compatible license
* https://github.com/jarro2783/cxxopts - MIT license

Make targets (that are mostly self-explanatory):

```
$ make
libmetro targets:
        clean
        build
        build-ubsan     (needs a clean)
        build-clang-tidy        (needs a clean)
        test
        install
        cpp-clean
        clang-analyze
        clang-format
```

The basics are build, test, and install. The tests ensure that the libsoundio defaults are cooperating on your system (initializing an output device with stereo output, etc.).

cpp-clean, build-ubsan, build-clang-tidy, and clang-analyze are different static analyzers and undefined behavior detection/correction tools. They're not mandatory (and in fact libmetro doesn't pass cleanly for most of them), but they provide important suggestions for making the code better.

clang-format is a code formatter tool and should be run before submitting a PR.
