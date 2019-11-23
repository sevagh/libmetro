# txt file formats

There are two experimental text file formats aimed at simplifying the configuration and maintenance of multiple metronomes, vs. coding each one in C++ (which can get tedious).

## Format 1

Fields are space-separated, lines are newline-separated. Comments start with `#`:

```
# number of beats in the measure
measure_length 4

# beats at index of the measure
0 sine,440.0,10.0 drum,73.42,100.0 drum,92.5,100.0 
1 sine,440.0,10.0 drum,73.42,50.0
2 sine,440.0,10.0 drum,73.42,65.0
3 sine,440.0,10.0 drum,73.42,50.0
```

Measures can be instantiated from text files:

```
auto metronome = metro::Metronome(bpm);

auto measure = metro::Measure("./sample_metronomes/accented_44_with_clicks.txt", metro::Measure::FileFormat::One);

metronome.add_measure(measure);
```

This is equivalent to the following C++ code written using libmetro's lower-level constructs:

```
auto metronome = metro::Metronome(bpm);

auto downbeat = metro::Note(metro::Note::Timbre::Drum, 73.42, 100.0) + 
                metro::Note(metro::Note::Timbre::Drum, 92.5, 100.0);
auto weakbeat = metro::Note(metro::Note::Timbre::Sine, 73.42, 50.0);
auto mediumbeat = metro::Note(metro::Note::Timbre::Drum, 73.42, 65.0);

auto click = metro::Note(metro::Note::Timbre::Sine, 440.0, 10.0);

metro::Measure measure(4);
measure[0] = click + downbeat;
measure[1] = click + weakbeat;
measure[2] = click + mediumbeat;
measure[3] = click + weakbeat;

metronome.add_measure(measure);
```

The bpm is not part of the metronome file, since it's assumed that the user will be changing bpm frequently.

Examples can be seen [here](https://github.com/sevagh/libmetro/tree/master/sample_metronomes). There is also an example program that parses and plays metronome txt files [here](https://github.com/sevagh/libmetro/blob/master/examples/from_file.cpp).

## Format 2

Suggestion from Victor. Fields are space-separated, lines are newline-separated. Comments start with `#`:

```
1 0 1 0 1 0
1 0 0 1 0 0
```

Measures can be instantiated from text files:

```
auto metronome = metro::Metronome(bpm);

auto measure = metro::Measure("./sample_metronomes/poly_32_format2.txt", metro::Measure::FileFormat::Two);

metronome.add_measure(measure);
```

In file format 2, the downbeats and beats are chosen for you. You're limited to 2 (although there's no reason one cannot add more timbres to [support additional overlaid rhythms](https://github.com/sevagh/libmetro/blob/master/src/fileparser.cpp#L10)).

Your power is in deciding how to overlay different beats of a polyrhythm.

Examples can be seen [here](https://github.com/sevagh/libmetro/tree/master/sample_metronomes), with suffix `_format2.txt`.

## Imperfect UX

There are many ways the UX can be improved in libmetro - visual metronome builder UIs, sheet music readers, etc.

I'm content to have pursued my goal of creating a generic enough C++ library, with some difficult bits taken care of (real-time audio, ticking timer, etc.), upon which anybody could build their desired metronome-building UI/UX.
