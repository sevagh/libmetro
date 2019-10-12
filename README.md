Libjungle is a library for constructing beat and rhythm-related programs - most likely UNIX-compatible - on the C++ command line.

Modern (c++2a) C++ std components are used liberally (vectors, lists, chrono, atomic, lambdas, threads, etc.).

Low-level constructs provided are:
- `jungle::event::EventFunc, EventCycle` - wraps lambdas and vectors of lambdas representing events that can be dispatched in a timer e.g. "play a beep"
- `jungle::tempo::Tempo` - wraps std::chrono, a precise (tested for clock accuracy within 5%) time ticker for metronome-style tasks
- jungle::audio::Engine, Stream - wraps SoundIo cross-platform real time audio functionality to emit sound
- jungle::audio::timbre::Timbre - wraps Stk to produce vectors of floats representing sounds (beeps, drum taps, etc.)
 
The syntax is partly inspired by Max/MSP metro+cycle objects.

End result code hides some implementation complexity (not all of it), but the code should be readable and obvious in the musical functionality it expresses.

E.g. here's a basic monotone click track at 100bpm:

```c++
auto ticker = jungle::tempo::Tempo(100);
auto click = jungle::audio::timbre::Pulse(440.0, 100.0);

auto continuous_clicks = jungle::EventCycle({
    [&]() {
      jungle::audio::timbre::play_on_stream(stream, {&click});
    },
});

ticker.register_event_cycle(continuous_clicks);
tempo.start();

// default blocking eventloop until Ctrl-C is pressed by the user - you can provide your own
jungle::audio::eventloop();
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
