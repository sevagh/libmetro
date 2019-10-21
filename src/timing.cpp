#include "libmetro.h"
#include "timing.h"
#include "audioengine.h"
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

metro_private::Tempo::Tempo(int bpm)
    : bpm(bpm)
    , engine(metro_private::AudioEngine())
    , ticker_on({true})
{
	if (!std::chrono::steady_clock::is_steady)
		throw std::runtime_error("std::chrono::steady_clock is unsteady on "
		                         "this platform");

	period_us[metro::NoteLength::Half] = std::chrono::duration_cast<std::chrono::microseconds>(
	    std::chrono::duration<double, std::micro>(2.0 * 1000000.0 * (60.0 / bpm)));
	period_us[metro::NoteLength::Quarter] = std::chrono::duration_cast<std::chrono::microseconds>(
	    std::chrono::duration<double, std::micro>(1000000.0 * (60.0 / bpm)));
	period_us[metro::NoteLength::Eighth] = std::chrono::duration_cast<std::chrono::microseconds>(
	    std::chrono::duration<double, std::micro>(1.0/2.0 * 1000000.0 * (60.0 / bpm)));
	period_us[metro::NoteLength::Sixteenth] = std::chrono::duration_cast<std::chrono::microseconds>(
	    std::chrono::duration<double, std::micro>(1.0/4.0 * 1000000.0 * (60.0 / bpm)));

	streams[metro::NoteLength::Half] = engine.new_outstream(period_us_2);
	streams[metro::NoteLength::Quarter] = engine.new_outstream(period_us_4);
	streams[metro::NoteLength::Eighth] = engine.new_outstream(period_us_8);
	streams[metro::NoteLength::Sixteenth] = engine.new_outstream(period_us_16);
}

void metro_private::Tempo::add_measure(metro::NoteLength note_length, metro::Measure& measure)
{
	streams[note_length].add_measure(measure);
}

void metro_private::Tempo::start()
{
	auto blocking_ticker_half_notes = [&](std::atomic<bool>& on) {
		while (on) {
			streams[metro::NoteLength::Half].play_next_note();
			metro::precise_sleep_us(period_us[metro::NoteLength::Half]);
		}
	};

	auto blocking_ticker_quarter_notes = [&](std::atomic<bool>& on) {
		while (on) {
			streams[metro::NoteLength::Quarter].play_next_note();
			metro::precise_sleep_us(period_us[metro::NoteLength::Quarter]);
		}
	};

	auto blocking_ticker_eighth_notes = [&](std::atomic<bool>& on) {
		while (on) {
			streams[metro::NoteLength::Eighth].play_next_note();
			metro::precise_sleep_us(period_us[metro::NoteLength::Eighth]);
		}
	};

	auto blocking_ticker_sixteenth_notes = [&](std::atomic<bool>& on) {
		while (on) {
			streams[metro::NoteLength::Sixteenth].play_next_note();
			metro::precise_sleep_us(period_us[metro::NoteLength::Sixteenth]);
		}
	};

	ticker_thread[metro::NoteLength::Half] = std::thread(blocking_ticker_half_notes, std::ref(ticker_on));
	ticker_thread[metro::NoteLength::Quarter] = std::thread(blocking_ticker_quarter_notes, std::ref(ticker_on));
	ticker_thread[metro::NoteLength::Eighth] = std::thread(blocking_ticker_eighth_notes, std::ref(ticker_on));
	ticker_thread[metro::NoteLength::Sixteenth] = std::thread(blocking_ticker_sixteenth_notes, std::ref(ticker_on));
}

void metro_private::Tempo::stop()
{
	ticker_on = false;
	for (size_t i = 0; i < 4; ++i)
		if (ticker_threads[i].joinable())
			ticker_threads[i].join();
}

metro_private::Tempo::~Tempo() { stop(); }
