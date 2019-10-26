#include "metronome.h"
#include "audioengine.h"
#include "libmetro.h"
#include "outstream.h"
#include <atomic>
#include <chrono>
#include <soundio/soundio.h>
#include <thread>
#include <vector>

metro::Metronome::Metronome(int bpm)
{
	p_impl = new metro_private::MetronomePrivate(bpm);
}

void metro::Metronome::add_measure(metro::NoteLength note_length,
                                   metro::Measure& measure)
{
	p_impl->add_measure(note_length, measure);
}

void metro::Metronome::start_and_loop()
{
	p_impl->start();
	p_impl->loop();
}

void metro::Metronome::start() { p_impl->start(); }

metro::Metronome::~Metronome()
{
	p_impl->stop();
	delete p_impl;
}

void metro::precise_sleep_us(std::chrono::microseconds dur_us)
{
	auto start_tick = std::chrono::steady_clock::now();
	while (std::chrono::duration_cast<std::chrono::microseconds>(
	           std::chrono::steady_clock::now() - start_tick)
	       < dur_us)
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
}

metro_private::MetronomePrivate::MetronomePrivate(int bpm)
    : bpm(bpm)
    , engine(metro_private::AudioEngine())
    , period_us_2(std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::duration<double, std::micro>(2.0 * 1000000.0
                                                    * (60.0 / bpm))))
    , period_us_4(std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::duration<double, std::micro>(1000000.0 * (60.0 / bpm))))
    , period_us_8(std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::duration<double, std::micro>(1.0 / 2.0 * 1000000.0
                                                    * (60.0 / bpm))))
    , period_us_16(std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::duration<double, std::micro>(1.0 / 4.0 * 1000000.0
                                                    * (60.0 / bpm))))
    , stream_2(engine.new_outstream(period_us_2))
    , stream_4(engine.new_outstream(period_us_4))
    , stream_8(engine.new_outstream(period_us_8))
    , stream_16(engine.new_outstream(period_us_16))
    , tickers_on({true})
{
	if (!std::chrono::steady_clock::is_steady)
		throw std::runtime_error("std::chrono::steady_clock is unsteady on "
		                         "this platform");
}

void metro_private::MetronomePrivate::add_measure(metro::NoteLength note_length,
                                                  metro::Measure& measure)
{
	switch (note_length) {
	case metro::NoteLength::Half:
		stream_2.add_measure(measure);
		break;
	case metro::NoteLength::Quarter:
		stream_4.add_measure(measure);
		break;
	case metro::NoteLength::Eighth:
		stream_8.add_measure(measure);
		break;
	case metro::NoteLength::Sixteenth:
		stream_16.add_measure(measure);
		break;
	}
}

void metro_private::MetronomePrivate::start()
{
	auto blocking_ticker_2 = [&](std::atomic<bool>& on) {
		stream_2.start();
		while (on) {
			std::thread([this]() { stream_2.play_next_note(); }).detach();
			metro::precise_sleep_us(period_us_2);
		}
	};

	auto blocking_ticker_4 = [&](std::atomic<bool>& on) {
		stream_4.start();
		while (on) {
			std::thread([this]() { stream_4.play_next_note(); }).detach();
			metro::precise_sleep_us(period_us_4);
		}
	};

	auto blocking_ticker_8 = [&](std::atomic<bool>& on) {
		stream_8.start();
		while (on) {
			std::thread([this]() { stream_8.play_next_note(); }).detach();
			metro::precise_sleep_us(period_us_8);
		}
	};

	auto blocking_ticker_16 = [&](std::atomic<bool>& on) {
		stream_16.start();
		while (on) {
			std::thread([this]() { stream_16.play_next_note(); }).detach();
			metro::precise_sleep_us(period_us_16);
		}
	};

	if (stream_2.has_measures())
		ticker_thread_2 = std::thread(blocking_ticker_2, std::ref(tickers_on));

	if (stream_4.has_measures())
		ticker_thread_4 = std::thread(blocking_ticker_4, std::ref(tickers_on));

	if (stream_8.has_measures())
		ticker_thread_8 = std::thread(blocking_ticker_8, std::ref(tickers_on));

	if (stream_16.has_measures())
		ticker_thread_16
		    = std::thread(blocking_ticker_16, std::ref(tickers_on));
}

void metro_private::MetronomePrivate::loop() { engine.eventloop(); }

void metro_private::MetronomePrivate::stop()
{
	tickers_on = false;
	if (ticker_thread_2.joinable())
		ticker_thread_2.join();
	if (ticker_thread_4.joinable())
		ticker_thread_4.join();
	if (ticker_thread_8.joinable())
		ticker_thread_8.join();
	if (ticker_thread_16.joinable())
		ticker_thread_16.join();
}

metro_private::MetronomePrivate::~MetronomePrivate() { stop(); }
