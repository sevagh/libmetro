### Additive demo

Here's an example of additive rhythms and polyrhythms from a traditional African song:

\htmlonly
<iframe width="764" height="430" src="https://www.youtube.com/embed/haGWi5lTibI" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
\endhtmlonly

This is my interpretation of the rhythm of the measure from 1:16-1:35, using the code in butour_ngale.cpp. It's a summing of several meters:

1. 6/8 is the stable drum beat, represented with 48 notes of EighthTriplets (8 x 6/8 manual repetitions)
2. The double-clapping is represented with SixteenthTriplets (96 notes, the first 48 are blank, 48-72 contain the double claps)
3. The syncopated single claps are represented with SixteenthTriplets (96 notes, the first 72 are blank, 72-96 contain the syncopated single claps)

butour_ngale.wav:

\htmlonly
<audio controls="1">
  <source src="./static/butour_ngale.wav"
          type="audio/wav">
  </source>
</audio>
\endhtmlonly
