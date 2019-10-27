### Getting started

The goal of libmetro is to simplify the development of specialized metronomes, e.g. simple, odd, compound, additive, and polyrhythmic click/drum tracks for practicing. Many of the examples and programs are adapted from the excellent [Bounce Metronome](https://bouncemetronome.com/audio/downloadable-audio-clips/audio-clips-time-signatures-additive-rhythms-and-polyrhythm) website.

The provided classes are Metronome, Note, and Measure. The bpm of the metronome should **always be given as the bpm of the quarter note**.

A Note is a convenience wrapper around a vector of floats representing a single sound - choices of timbre include `Timbre::{Sine, Drum}`.

A Measure is a convenience wrapper around a vector of Notes representing a measure. Measures are added to a metronome with their note length, represented with the enum `NoteLength::{Half, Quarter, QuarterTriplet, Eighth, EighthTriplet, Sixteenth, SixteenthTriplet}`.

### Basic usage

Here's a simple example of an accented 4/4 beat with sine waves:

```
#include "libmetro.h"

int bpm = 100;
auto metronome = metro::Metronome(bpm);

auto downbeat = metro::Note(metro::Note::Timbre::Sine, 540.0, 100.0);
auto weakbeat = metro::Note(metro::Note::Timbre::Sine, 350.0, 50.0);
auto mediumbeat = metro::Note(metro::Note::Timbre::Sine, 440.0, 65.0);

metro::Measure accented_44(4);
accented_44[0] = downbeat;
accented_44[1] = weakbeat;
accented_44[2] = mediumbeat;
accented_44[3] = weakbeat;

metronome.add_measure(metro::Measure::NoteLength::Quarter, accented_44);
metronome.start_and_loop();
```

\htmlonly
<audio controls="1">
  <source src="./static/accented_4_4_demo.wav"
          type="audio/wav">
  </source>
</audio>
\endhtmlonly

Here's a more complex example of a 3:2 polyrhythm/cross-rhythm. The triples are played with snares (+ hihat for the downbeat), and the duples are played with cowbell (+ tambourine for the downbeat):

```
#include "libmetro.h"

int bpm = 100;
auto metronome = metro::Metronome(bpm);

auto poly1 = metro::Measure(2);
poly1[0] = metro::Note(metro::Note::Timbre::Drum, 185.0, 100.0)
           + metro::Note(metro::Note::Timbre::Drum, 207.65, 100.0);
poly1[1] = metro::Note(metro::Note::Timbre::Drum, 207.65, 50.0);

metronome.add_measure(metro::Measure::NoteLength::Quarter, poly1);

auto poly2 = metro::Measure(3);
poly2[0] = metro::Note(metro::Note::Timbre::Drum, 73.42, 100.0)
           + metro::Note(metro::Note::Timbre::Drum, 92.50, 100.0);
poly2[1] = metro::Note(metro::Note::Timbre::Drum, 73.42, 50.0);
poly2[2] = metro::Note(metro::Note::Timbre::Drum, 73.42, 50.0);

metronome.add_measure(
    metro::Measure::NoteLength::QuarterTriplet, poly2);
metronome.start_and_loop();
```

\htmlonly
<audio controls="1">
  <source src="./static/poly_3_2_demo.wav"
          type="audio/wav">
  </source>
</audio>
\endhtmlonly