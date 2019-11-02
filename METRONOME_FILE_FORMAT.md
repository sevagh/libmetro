### The Metronome txt file format

I'm drawing some (loose) inspiration from: https://github.com/urinieto/harmonixset. Their description of beats and downbeats is as follows:

>beats_and_downbeats: Directory with a tab-separated file for each track in the dataset, with the following three fields per line containing beats and downbeats:
>
>    beat_time_stamp: The placement of the beat in seconds (and downbeat, if beat_position_in_bar = 1).
>
>    beat_position_in_bar: The number of beat within a bar (when 1, the beat also represents a downbeat).
>
>    bar_number: The number of the bar.

The libmetro file format is as follows. Fields are space-separated, lines are newline-separated:

```
$ cat sample_metronomes/accented_44_with_clicks.txt
# number of beats in the measure
measure_length 4

# beats at index of the measure
0 sine,440.0,10.0 drum,73.42,100.0 drum,92.5,100.0 
1 sine,440.0,10.0 drum,73.42,50.0
2 sine,440.0,10.0 drum,73.42,65.0
3 sine,440.0,10.0 drum,73.42,50.0
```

The beats `sine,440.0,10.0` and `drum,73.42,100.0` represent:

* Timbre (using stk/Sine or stk/Drummer)
* Frequency (for stk/Drummer, this is converted to a general MIDI percussion instrument)
* Volume (in percent)

The above metronome contains:

1. A 10% volume 440.0Hz sine wave click at every beat of the measure
2. A downbeat at 0 consisting of a 100% volume cowbell + 100% volume tambourine
3. Weak beats at 1 and 3 consisting of a 50% volume cowbell
4. A 'medium beat' at 2 consisting of a 65% volume cowbell

The bpm is not part of the metronome file, since it's assumed that the user will be changing bpm frequently.

Examples can be seen in [./sample_metronomes](./sample_metronomes).
