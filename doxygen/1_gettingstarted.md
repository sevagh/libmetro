### Getting started

The goal of libmetro is to simplify the development of specialized metronomes, e.g. simple, odd, compound, additive, and polyrhythmic click/drum tracks for practicing.

The provided classes are Metronome, Note, and Measure.

A Note is a convenience wrapper around a vector of floats representing a single sound - choices of timbre include `Timbre::{Sine, Drum}`.

A Measure is a convenience wrapper around a vector of Notes representing a measure. Measures are added to a metronome.

Measures can be registered onto a Metronome object, and the bpm of the metronome determines the frequency at which the notes in the measures are cycled through.

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

metronome.add_measure(accented_44);
metronome.start_and_loop();
```

This is what it sounds like:

\htmlonly
<audio controls="1">
  <source src="./static/accented_4_4_demo.wav"
          type="audio/wav">
  </source>
</audio>
\endhtmlonly
