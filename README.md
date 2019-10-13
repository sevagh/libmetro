Libjungle is a library for constructing beat and rhythm-related programs - most likely UNIX-compatible - on the C++ command line.

Modern (c++2a) C++ std components are used liberally (vectors, lists, chrono, atomic, lambdas, threads, etc.).

Low-level constructs provided are:
- `jungle::event::EventFunc, EventCycle` - wraps lambdas and vectors of lambdas representing events that can be dispatched in a timer e.g. "play a beep"
- `jungle::tempo::Tempo` - wraps std::chrono, a precise (tested for clock accuracy within 5%) time ticker for metronome-style tasks
- jungle::audio::Engine, Stream - wraps SoundIo cross-platform real time audio functionality to emit sound
- jungle::audio::timbre::Timbre - wraps Stk to produce vectors of floats representing sounds (beeps, drum taps, etc.)
 
The syntax is partly inspired by Max/MSP metro+cycle objects.

End result code hides some implementation complexity, but a major goal of this project is that the code should be *obvious* in the musical functionality it expresses even by non-programmers.

E.g. here's an annotated basic monotone click track at 100bpm:

```c++
#include <libjungle.h>

int main() {
    // create a 100bpm tempo timer
    auto ticker = jungle::tempo::Tempo(100);

    // create an "audio engine" (really just a thin wrapper around SoundIo structs)
    // use the period of the timer to pick an optimal output stream latency
    auto audio_engine = jungle::audio::Engine();
    auto stream = audio_engine.new_stream(ticker.period_us);

    // create a 440Hz sine wave at 100% volume
    auto click = jungle::audio::timbre::Pulse(440.0, 100.0);

    // create and register a cycle that only plays the above click repeatedly
    auto continuous_clicks = jungle::EventCycle({
        [&]() {
          jungle::audio::timbre::play_on_stream(stream, {&click});
        },
    });

    ticker.register_event_cycle(continuous_clicks);

    // start the ticker and block until ctrl-c is pressed
    // you can provide your own blocking functions
    ticker.start();
    jungle::audio::eventloop();

    return 0;
}
```

Another good example is that I worked with [my musician friend](https://www.instagram.com/nazguitar/?hl=en) to define "correct" time signature metronomes, to the point where he was dictating what frequency and volume of beats I should use to emphasize up/downbeats:

```c++
```

Implemented programs include:

- A 4/4 metronome (link)
- A 4/4 drum track (link)
- A tap-tempo adjustable click track
- A polyrhyhtmic metronome (3/3 over 4/4, each one pausable) - link
- A schedulable metronome (to practise songs where the time signature and/or tempo changes midway)

### Instructions

The following tools are used for ensuring code quality:
- googletest (for unit tests)
- valgrind for memory leak checks
- clang-tidy
- clang-format
- cppclean
- ubsan
