#include "libmetro.h"
#include <atomic>
#include <chrono>
#include <thread>

void metro::precise_sleep_us(std::chrono::microseconds dur_us)
{
	auto start_tick = std::chrono::steady_clock::now();
	while (std::chrono::duration_cast<std::chrono::microseconds>(
	           std::chrono::steady_clock::now() - start_tick)
	       < dur_us)
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
}

metro::Tempo::Tempo(int bpm)
    : bpm(bpm)
    , ticker_on({true})
{
	if (!std::chrono::steady_clock::is_steady)
		throw std::runtime_error("std::chrono::steady_clock is unsteady on "
		                         "this platform");

	period_us = std::chrono::duration_cast<std::chrono::microseconds>(
	    std::chrono::duration<double, std::micro>(1000000.0 * (60.0 / bpm)));
}

void metro::Tempo::register_measure(metro::Measure& measure)
{
	measures.push_back(&measure);
}

void metro::Tempo::start()
{
	auto blocking_ticker = [&](std::atomic<bool>& on) {
		while (on) {
			for (auto m : measures)
				m->play_next_quarter_note();

			precise_sleep_us(period_us);
		}
	};

	ticker_thread = std::thread(blocking_ticker, std::ref(ticker_on));
}

void metro::Tempo::stop()
{
	ticker_on = false;
	if (ticker_thread.joinable())
		ticker_thread.join();
}

metro::Tempo::~Tempo() { stop(); }
