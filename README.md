Libjungle is a C++ library for developing beat and rhythm-related programs. I chose the name `jungle` because:

1. the jungle genre (electronic music) emphasizes rhythm, beat, tempo, syncopation, synthesized sounds - all things that I'd like to incorporate into libjungle
2. jungles (real tropical jungles in nature) make me think of an orangutan slamming funky beats on bongo drums and coconut shells - very cool

As of now, the focus of libjungle is _synthesis_, i.e. the programmatic creation of rhythms and beats, namely through a variety of specialized metronomes. This will be presented as my final project in [MUMT-306](https://www.music.mcgill.ca/~gary/306/).

### Goal

The primary goal of libjungle is to make the musical function of code that uses it **clear and obvious** to non-programmers. E.g. here's an annotated basic monotone click track at 100bpm:

```c++
#include <libjungle/libjungle.h>
#include <libjungle/libjungle_synthesis.h>

int main() {
    // create a 100bpm tempo ticker
    auto ticker = jungle::core::tempo::Tempo(100);

    // create an audio engine with an output stream
    // use the period of the timer to pick an optimal output stream latency
    auto audio_engine = jungle::core::audio::Engine();
    auto stream = audio_engine.new_outstream(ticker.period_us);

    // create a 440Hz sine tone at 100% volume, to use as a beep/click
    auto click = jungle::synthesis::timbre::Pulse(440.0, 100.0);

    // create and register a cycle that only plays the above click
    auto continuous_clicks = jungle::EventCycle({
        [&]() {
          jungle::synthesis::timbre::play_on_stream(stream, {&click});
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

### Documentation

The following tables describe libjungle's features at a high level, separated by namespace. Read the linked header files to find more details on constructors, members, etc.

#### Namespace jungle::core

Header: [libjungle/libjungle.h](./include/libjungle/libjungle.h)

| Component | Description |
| ----------- | -------- |
| `SampleRateHz` | Global sample rate for all libjungle components |
| `event::Event` | Alias for a `void()` function pointer<br/>suggestion for supporting arguments: [lambda captures](https://en.cppreference.com/w/cpp/language/lambda) |
| `class event::EventCycle` | Wraps a provided vector of Events and cycles through them<br/>Events are dispatched in the background using [thread detach](https://en.cppreference.com/w/cpp/thread/thread/detach) |
| `void tempo::precise_sleep_us(dur_us)` | A function for microsecond-precision sleep<br>Strategy is measure elapsed time and sleep 1ns in a loop<br/>Accuracy tested [here](./test/clock_accuracy.cpp) |
| `std::chrono::microseconds tempo::bpm_to_us(bpm)`<br/>`int tempo::us_to_bpm(us)` | Convert between bpm and period in microseconds |
| `class tempo::Tempo` | Create a timer executor of EventCycles with us precision<br/>This runs in the background so you need to block after `start()` |
| `class audio::Engine` | Wraps Soundio structs for low level audio functionality |
| `class audio::Engine::OutStream` | A nested class of Engine containing a Soundio struct for an output stream<br/>Note: only an engine can create OutStreams |

#### Namespace jungle::synthesis

Header: [libjungle/libjungle_synthesis.h](./include/libjungle/libjungle_synthesis.h)

| Component | Description |
| --------- | ----------- |
| `class timbre::Timbre` | An abstract base class for stk-based sound generators<br/>Generated stk samples are stored in a `std::vector<float>` |
| `void timbre::play_on_stream(OutStream, {Timbre1, Timbre2, ...})` | A function to play a list of Timbres (i.e. sounds) on the output stream<br/>This fills a ringbuffer with audio data which is read from the real-time audio callback |
| `class timbre::Pulse` | Implements Timbre, creates a sine tone<br/>Wraps [stk/SineWave.h](https://github.com/thestk/stk/blob/master/include/SineWave.h) |
| `class timbre::Drum` | Implements Timbre, creates MIDI drum sounds<br/>Wraps [stk/Drummer.h](https://github.com/thestk/stk/blob/master/include/Drummer.h) |
| `map<string, EventCycle> metronome::time_signature_mappings` | A map of implemented metronomes, key is time signature<br/>E.g. the common time metronome has key "4/4" |
| `EventCycle metronome::metronome_common_time` | Creates a 4/4 metronome event cycle<br/>Intended to be registered on a Tempo instance |

### Programs

Some [programs](./programs) are provided that demonstrate example metronome applications.

| Program | Arguments | Description |
| ------- | --------- | ----------- |
| [drum_track_4_4](./programs/drum_track_4_4.cpp) | bpm | A 4/4 drum track (hihat, snare, bass) |
| [tap_tempo](./programs/tap_tempo.cpp) |  | Measures user keypresses bpm and creates a click track |
| metronome | time_signature bpm  | Plays common metronome time signatures (3/4, 4/4, 7/4, etc.) |
| poly_metronome | time_sig_a time_sig_b bpm  | Overlays two time signatures, each one mutable at a time<br/>intended for practicing polyrhythms |
| schedule_metronome | time_sig_a measures_a time_sig_b measures_b bpm  | Alternates between given time signatures, duration in measures |

Note that the creation of a "perfect metronome" is a non-goal of libjungle. Users should feel empowered to create any sort of metronome they need on the fly, without many lines of code.

### Dev guide

*wip*

Modern (c++2a) std components are used liberally throughout the codebase (vectors, lists, chrono, atomic, lambdas, threads, mutexes, call_once, etc.).

The following tools are used for ensuring code quality:
- googletest (for unit tests)
- valgrind for memory leak checks
- clang-tidy
- clang-format
- cppclean
- ubsan


### Future plans

**N.B.** these are tentative!

The second part of libjungle will be _analysis_, e.g. recording audio and measuring the tempo, beats, onsets to help musicians practice. If possible, I'd like to present the future features of libjungle in [MUMT-307](https://www.music.mcgill.ca/~gary/307/).
