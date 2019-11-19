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

auto measure = metro::Measure("./sample_metronomes1/accented_44_with_clicks.txt", metro::Measure::FileFormat::One);

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

Examples can be seen [here](https://github.com/sevagh/libmetro/tree/master/sample_metronomes1). There is also an example program that parses and plays metronome txt files [here](https://github.com/sevagh/libmetro/blob/master/examples/from_file.cpp).

## Format 2

Fields are space-separated, lines are newline-separated. Comments start with `#`:

```
1 0 1 0 1 0
1 0 0 1 0 0
```

Measures can be instantiated from text files:

```
auto metronome = metro::Metronome(bpm);

auto measure = metro::Measure("./sample_metronomes2/poly_32.txt", metro::Measure::FileFormat::Two);

metronome.add_measure(measure);
```

In file format 2, the downbeats and beats are chosen for you. You decide how to overlay different beats of a polyrhythm.

Examples can be seen [here](https://github.com/sevagh/libmetro/tree/master/sample_metronomes2).

## Future ideas

More ideas:

### Metrolang

A more formal, tiny "programming language" that compiles down to a C++ binary with libmetro. Language spec would be more expressive than the current text file formats. The implementation is way outside the scope of this course but it's a fun thought experiment.

Example of the pedagogical 4:3 (can be seen in the Polyrhythm doc) (**n.b.! this isn't implemented anywhere, just in my dreams**):

```
notes {
    click = sine,440.0,20.0
    d1 = drum,73.42,100.0 drum,92.5,100.0
    w1 = drum,73.42,50.0
    d2 = drum,207.65,100.0 drum,185.0,100.0
    w2 = drum,207.65,50.0
}

# 4s
measure[12] {
    _ click
    0 d1
    3 w1
    6 w1
    9 w1
}x2

# 3s
measure[12] {
    _ click
    0 d2
    4 w2
    8 w2
}x2

# 4:3
measure[12] {
    _ click
    0 d1 d2
    3 w1
    4 w2
    6 w1
    8 w2
    9 w1
}x2

# only clicks
measure[12] {
    _ click
}x2
```

Many different people have given me their take on what an optimal UI/UX would look like. To me this validates my goal that producing a correct, generally applicable C++ library (upon which anybody could build their desired metronome builder) was the right move.
