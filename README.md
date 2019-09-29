Version will be 0.0.1 for a while - very experimental codebase.

### Third-party libraries used, and why

* stk - https://github.com/thestk/stk - used to generate tones, beeps, drum taps, etc. as arrays of floats to play through libsoundio
* libsoundio - https://github.com/andrewrk/libsoundio - a high-quality, modern, real-time cross-platform audio library, to avoid interfacing with platform-specific audio code (e.g. ALSA, CoreAudio)

### Learning resources

These articles gave me some good tips on how I should approach real-time audio programming:

* http://www.rossbencina.com/code/real-time-audio-programming-101-time-waits-for-nothing
* http://atastypixel.com/blog/four-common-mistakes-in-audio-development/

I found the stk examples and libsoundio examples a good enough starting point to mold into working code.

### Notes

* my [fork of stk](https://github.com/sevagh/stk) has been compiled with `typedef float StkFloat` (instead of double) to match the floats I use in libjungle (enforced by libsoundio)
