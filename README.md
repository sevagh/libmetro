Libmetro is a C++ library for developing interesting metronome variants.

**link to github-pages for complete API docs and real demos are coming soon**

The goal of libmetro is to make the development of specialized metronomes, e.g. simple, odd, compound, additive, and polyrhythmic click/drum tracks for practicing. Many of the examples and programs are adapted from the excellent [Bounce Metronome](https://bouncemetronome.com/audio/downloadable-audio-clips/audio-clips-time-signatures-additive-rhythms-and-polyrhythm) website.

The provided classes are Metronome, Note, and Measure. The bpm of the metronome should **always be given as the bpm of the quarter note**. There is a global const `SampleRateHz = 48000`, provided to allow the user to align their sample rates accordingly. This was a design choice for simplicity.

A Note is a convenience wrapper around a vector of floats representing a single sound - choices of timbre include `Timbre::{Sine, Drum}`. In the case of Drum, the frequency is the [general MIDI drum note](https://en.wikipedia.org/wiki/General_MIDI#Percussion), e.g. 56.0 for cowbell.

A Measure is a convenience wrapper around a vector of Notes representing a measure. Measures are added to a metronome with their note length, represented with the enum NoteLength::{Half, Quarter, QuarterTriplet, Eighth, EighthTriplet, Sixteenth}. Of course, other values are possible - I only added a minimal subset at the start.

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

Here's a more complex example of a 3:2 polyrhythm/cross-rhythm. The 3/8 is played with snares (+ hihat for the downbeat), and the 2/4 is played with cowbell (+ tambourine for the downbeat):

```c++
#include "libmetro.h"

int bpm = 100;
auto metronome = metro::Metronome(bpm);

auto two_over_four = metro::Measure(2);
two_over_four[0] = metro::Note(metro::Timbre::Drum, 54.0, 100.0)
                   + metro::Note(metro::Timbre::Drum, 56.0, 100.0);
two_over_four[1] = metro::Note(metro::Timbre::Drum, 56.0, 50.0);

metronome.add_measure(metro::NoteLength::Quarter, two_over_four); // regular quarter notes

auto three_over_four = metro::Measure(3);
three_over_four[0] = metro::Note(metro::Timbre::Drum, 38.0, 100.0)
                     + metro::Note(metro::Timbre::Drum, 42.0, 100.0);
three_over_four[1] = metro::Note(metro::Timbre::Drum, 38.0, 50.0);
three_over_four[2] = metro::Note(metro::Timbre::Drum, 38.0, 50.0);

metronome.add_measure(
    metro::NoteLength::QuarterTriplet, three_over_four); // note the QuarterTriplet note length

metronome.start_and_loop();
```

Listen: [3:2 polyrhythm, 100bpm](./.github/poly_3_2_demo.wav)

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
