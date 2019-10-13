#include "libjungle.h"
#include <chrono>
#include <iostream>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

int _kbhit()
{
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

int main()
{
	std::cout << "press any key repeatedly to set the tempo to your key press "
	             "rate\nonce your keypresses produce a stable tempo (±5bpm), "
	             "the bpm will be chosen and the input loop will exit\nctrl-c "
	             "at any time to abort"
	          << std::endl;

	int user_bpm = 0;
	int last_user_bpm = 0;
	int last_hit = 0;
	auto last_hit_time = std::chrono::steady_clock::now();

	while (true) {
		int hit = _kbhit();
		auto hit_time = std::chrono::steady_clock::now();

		if (hit != last_hit) {
			last_hit = hit;
			auto hit_delta_us
			    = std::chrono::duration_cast<std::chrono::microseconds>(
			        hit_time - last_hit_time);
			user_bpm = jungle::tempo::us_to_bpm(hit_delta_us);

			if (std::abs(last_user_bpm - user_bpm) < 5) {
				std::cout << "stable bpm chosen: " << user_bpm << std::endl;
				break;
			}

			std::cout << "bpm: " << user_bpm << std::endl;
			last_user_bpm = user_bpm;
			last_hit_time = hit_time;
		}

		jungle::tempo::precise_sleep_us(std::chrono::microseconds(1));
	}

	auto tempo = jungle::tempo::Tempo(user_bpm);

	auto audio_engine = jungle::audio::Engine();
	auto stream = audio_engine.new_stream(tempo.period_us);

	std::cout << "init audio engine" << std::endl;

	auto beep = jungle::audio::timbre::Pulse(440.0, 100.0);

	auto beeps = jungle::event::EventCycle({
	    [&]() { jungle::audio::timbre::play_on_stream(stream, {&beep}); },
	});

	tempo.register_event_cycle(beeps);
	tempo.start();

	audio_engine.eventloop();

	return 0;
}
