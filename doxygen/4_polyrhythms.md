### Polyrhythm

Polyrhythm is the original motivator of libmetro. Polyrhythm as defined by britannica.com is:

>Polyrhythm, also called Cross-rhythm, the simultaneous combination of contrasting rhythms in a musical composition. Rhythmic conflicts, or cross-rhythms, may occur within a single metre (e.g., two eighth notes against triplet eighths) or may be reinforced by simultaneous combinations of conflicting metres.

I first learned of polyrhythm through metal bands (Meshuggah, Animals as Leaders, Periphery, Plini, etc.):

\htmlonly
<iframe width="434" height="192" src="https://www.youtube.com/embed/T4gTgM0UepQ?start=130" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
\endhtmlonly

Polyrhythm features in African music (and may have even originated there):

\htmlonly
<iframe width="434" height="192" src="https://www.youtube.com/embed/haGWi5lTibI" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
\endhtmlonly

EDM example (not 100% certain on this one):

\htmlonly
<iframe width="434" height="192" src="https://www.youtube.com/embed/V31kBcV3k38" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
\endhtmlonly

### libmetro representation

Polyrhythms are expressed rather simply. They need to be manually constructed using the least common multiple of both rhythms, which is a familiar exercise to anyone studying polyrythms or following basic tutorials (as I did - linked right below).

Resources, tutorials:

* https://bouncemetronome.com/video-resources/polyrhythms
* https://blog.landr.com/use-polyrhythms-get-perfect-beat/
* https://www.youtube.com/watch?v=_37pioTK_gA
* https://www.youtube.com/watch?v=b29H5RLcijs

As before, the metronome files below can be viewed [here](https://github.com/sevagh/libmetro/tree/master/sample_metronomes).

#### 3:2 example

To create a 3:2, we need to find the LCM of 3 and 2, which is 6, spread the triple at indices 0,2,4, and the duple at indices 0,3:

poly_32.txt:

```
measure_length 6

0 sine,440.0,10.0 drum,73.42,100.0 drum,92.5,100.0 drum,207.65,100.0 drum,185.0,100.0
1 sine,440.0,10.0
2 sine,440.0,10.0 drum,73.42,50.0
3 sine,440.0,10.0 drum,207.65,50.0
4 sine,440.0,10.0 drum,73.42,50.0
5 sine,440.0,10.0
```

You'll see see I added sine wave beeps/clicks to make clear how the triples and duples are overlaid on 6 beats:

3:2, 250bpm:

\htmlonly
<audio controls="1">
  <source src="./static/poly_32.wav"
          type="audio/wav">
  </source>
</audio>
\endhtmlonly

2:3 (inverted timbres of 3:2), 250bpm:

\htmlonly
<audio controls="1">
  <source src="./static/poly_23.wav"
          type="audio/wav">
  </source>
</audio>
\endhtmlonly

#### 4:3 with a pedagogical twist

There's a fully-fledged [pure-C++ 4:3 example](https://github.com/sevagh/libmetro/blob/master/examples/poly_43.cpp). where one can specify how many measures of 4, 3, 4:3, and only clicks (no beats) to play.

The idea is that you could practice X measures of 4, Y measures of 3, Z measures of 4:3, and W measures of only clicks (on which you're expected to correctly play the 4:3), repeated over and over. This can perhaps be of value to drummers.

Note that this concept is expressible with the text file method, but requires one to copy/paste blocks of measures (and change the indices manually), which is tedious.

Test run with parameters 2 2 2 2 300, i.e. 2 measures of 4, 2 measures of 3, 2 measures of 4:3, and 2 "blank" measures of just clicks, at 300bpm:

\htmlonly
<audio controls="1">
  <source src="./static/poly_43.wav"
          type="audio/wav">
  </source>
</audio>
\endhtmlonly

