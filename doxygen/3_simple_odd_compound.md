# Simple, odd, compound

These examples are simple, so I don't cover exhaustive cases. E.g., going from 5/4 to 7/4 just means adding 2 more weakbeats. The metronome files used to generate the clips can be viewed [here](https://github.com/sevagh/libmetro/tree/master/sample_metronomes1) and [here](https://github.com/sevagh/libmetro/tree/master/sample_metronomes2).

Resources, tutorials:

* https://www.dummies.com/art-center/music/piano/understanding-simple-and-compound-time-signatures/
* https://www.studybass.com/lessons/reading-music/time-signatures/
* https://www.musictheory.net/lessons/15

## bpm caveat

As mentioned previously, the bpm of the metronome determines the frequency at which a measure's notes are played. This may not correspond to the musically correct bpm. E.g. for 9/8 (which you'll see below), libmetro doesn't do any triplet divisions but steps through the measure the same as any other.

## simple

simple_34.txt:
```
measure_length 3

0 drum,207.65,100.0 drum,185.0,100.0
1 drum,207.65,40.0
2 drum,207.65,40.0
```

3/4, 120bpm

\htmlonly
<audio controls="1">
  <source src="./static/34.wav"
          type="audio/wav">
  </source>
</audio>
\endhtmlonly

## odd

odd_54.txt:
```
measure_length 5

0 drum,207.65,100.0 drum,185.0,100.0
1 drum,207.65,40.0
2 drum,207.65,40.0
3 drum,207.65,40.0
4 drum,207.65,40.0
```

5/4, 170bpm

\htmlonly
<audio controls="1">
  <source src="./static/54.wav"
          type="audio/wav">
  </source>
</audio>
\endhtmlonly

## compound

compound_98.txt:
```
measure_length 9

0 drum,207.65,100.0 drum,185.0,100.0
1 drum,207.65,40.0
2 drum,207.65,40.0

3 drum,207.65,65.0 drum,185.0,50.0
4 drum,207.65,40.0
5 drum,207.65,40.0

6 drum,207.65,65.0 drum,185.0,50.0
7 drum,207.65,40.0
8 drum,207.65,40.0
```

9/8, 240bpm

\htmlonly
<audio controls="1">
  <source src="./static/98.wav"
          type="audio/wav">
  </source>
</audio>
\endhtmlonly
