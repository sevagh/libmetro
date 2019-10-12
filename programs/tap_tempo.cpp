#include <stdio.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include "libjungle.h"
#include <chrono>

int _kbhit() {
	/**
	 Linux (POSIX) implementation of _kbhit().
	 Morgan McGuire, morgan@cs.brown.edu

	 taken from: http://www.flipcode.com/archives/_kbhit_for_Linux.shtml
	 */
    static const int STDIN = 0;
    static bool initialized = false;

    if (!initialized) {
        // Use termios to turn off line buffering
        termios term;
        tcgetattr(STDIN, &term);

	// hide echo to read but not print user inputs
        term.c_lflag &= ~(ECHO | ICANON);
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytes_waiting;
    ioctl(STDIN, FIONREAD, &bytes_waiting);
    return bytes_waiting;
}

int main(int argc, char** argv)
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " bpm" << std::endl;
		exit(1);
	}

	int initial_bpm = std::stoi(argv[1]);
	auto tempo = jungle::tempo::Tempo(initial_bpm);

	std::cout << "init " << initial_bpm << "bpm tempo ticker - press any key repeatedly to adjust the tempo to your key press rate" << std::endl;

	auto audio_engine = jungle::audio::Engine();
	auto stream = audio_engine.new_stream(tempo.get_period_us());

	std::cout << "init audio engine" << std::endl;

	auto beep = jungle::audio::timbre::Pulse(440.0, 100.0);

	auto beeps = jungle::event::EventCycle({
		    [&]() {
			    jungle::audio::timbre::play_on_stream(stream, {&beep});
		    },
	});

	tempo.register_event_cycle(beeps);

	tempo.start();

	int last_hit = 0;
	auto last_hit_time = std::chrono::steady_clock::now();

	while (true) {
		int hit = _kbhit();
		auto hit_time = std::chrono::steady_clock::now();

		if (hit != last_hit) {
			last_hit = hit;
			auto hit_delta_us = std::chrono::duration_cast<std::chrono::microseconds>(hit_time - last_hit_time);
			auto user_bpm = jungle::tempo::us_to_bpm(hit_delta_us);

			std::cout << "user input bpm: " << user_bpm << std::endl;

			last_hit_time = hit_time;
			tempo.set_bpm(user_bpm);

			auto stream_new_latency = jungle::audio::pick_best_latency(hit_delta_us);
			stream.set_latency_s(stream_new_latency);
		}

		jungle::tempo::precise_sleep_us(std::chrono::microseconds(1));
	}

	printf("\nDone.\n");

	return 0;
}
