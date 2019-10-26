Libmetro is a C++ library for developing interesting metronome variants. This will be presented as my final project in [MUMT-306](https://www.music.mcgill.ca/~gary/306/). Note that the files in `src/` are very granular for clarity and easier grading. It may be beneficial to compile times to keep source code in less, larger cpp files.

### Goal

The primary goal of libmetro is to make the musical function of code that uses it **clear and obvious** to non-programmers. E.g. here's an annotated basic monotone click track at 100bpm:

```c++
TODO
```

### Programs

Some [programs](./programs) are provided that demonstrate example metronome applications.

| Program | Arguments | Description |
| ------- | --------- | ----------- |
| [drum_track_4_4](./programs/drum_track_4_4.cpp) | bpm | A 4/4 drum track (hihat, snare, bass) |
| [tap_tempo](./programs/tap_tempo.cpp) |  | Measures user keypresses bpm and creates a click track |
| metronome | time_signature bpm  | Plays common metronome time signatures (3/4, 4/4, 7/4, etc.) |
| poly_metronome | time_sig_a time_sig_b bpm  | Overlays two time signatures, each one mutable at a time<br/>intended for practicing polyrhythms |
| schedule_metronome | time_sig_a measures_a time_sig_b measures_b bpm  | Alternates between given time signatures, duration in measures |

Note that the creation of a one-size-fits-all, perfect metronome is a non-goal of libmetro. Users should feel empowered to create any sort of metronome they need on the fly, without many lines of code.

### Dev guide

*wip*

Modern (c++2a) std components are used liberally throughout the codebase (vectors, lists, chrono, atomic, lambdas, threads, mutexes, call_once, etc.).

The following tools are used for ensuring code quality:
- googletest (for unit tests)
- valgrind for memory leak checks
- clang-tidy
- clang-format
- cppclean
- ubsan
