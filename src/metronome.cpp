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

static std::chrono::microseconds bpm2period(metro::NoteLength ratio, int bpm)
{
	switch (ratio) {
	case metro::NoteLength::Half:
		return std::chrono::duration_cast<std::chrono::microseconds>(
		    std::chrono::duration<double, std::micro>(2.0 * 1000000.0
		                                              * (60.0 / bpm)));
	case metro::NoteLength::Quarter:
		return std::chrono::duration_cast<std::chrono::microseconds>(
		    std::chrono::duration<double, std::micro>(1000000.0 * (60.0 / bpm)));
	case metro::NoteLength::QuarterTriplet:
		return std::chrono::duration_cast<std::chrono::microseconds>(
		    std::chrono::duration<double, std::micro>(2.0 / 3.0 * 1000000.0
		                                              * (60.0 / bpm)));
	case metro::NoteLength::Eighth:
		return std::chrono::duration_cast<std::chrono::microseconds>(
		    std::chrono::duration<double, std::micro>(1.0 / 2.0 * 1000000.0
		                                              * (60.0 / bpm)));
	case metro::NoteLength::EighthTriplet:
		return std::chrono::duration_cast<std::chrono::microseconds>(
		    std::chrono::duration<double, std::micro>(1.0 / 3.0 * 1000000.0
		                                              * (60.0 / bpm)));
	case metro::NoteLength::Sixteenth:
		return std::chrono::duration_cast<std::chrono::microseconds>(
		    std::chrono::duration<double, std::micro>(1.0 / 4.0 * 1000000.0
		                                              * (60.0 / bpm)));
	default:
		return std::chrono::microseconds(0);
	}
}

metro_private::MetronomePrivate::MetronomePrivate(int bpm)
    : bpm(bpm)
    , engine(metro_private::AudioEngine())
    , tickers_on({true})
{
	if (!std::chrono::steady_clock::is_steady)
		throw std::runtime_error("std::chrono::steady_clock is unsteady on "
		                         "this platform");
}

void metro_private::MetronomePrivate::add_measure(metro::NoteLength note_length,
                                                  metro::Measure& measure)
{

	std::map<metro::NoteLength, NoteTicker>::iterator it
	    = tickers.find(note_length);
	if (it != tickers.end()) {
		auto ticker = it->second;
		ticker.stream->add_measure(measure);
	}
	else {
		NoteTicker ticker;

		ticker.note_length = note_length;
		ticker.period_us = bpm2period(note_length, bpm);
		ticker.stream = engine.new_outstream(ticker.period_us);
		ticker.stream->add_measure(measure);
		ticker.ticker_thread = nullptr;
		tickers[note_length] = ticker;
	}
}

void metro_private::MetronomePrivate::start()
{

	for (auto& [key, ticker] : tickers) {
		auto blocking_ticker = [&](std::atomic<bool>& on) {
			ticker.stream->start();
			while (on) {
				std::thread([&]() { ticker.stream->play_next_note(); }).detach();
				metro::precise_sleep_us(ticker.period_us);
			}
		};

		if (ticker.stream->has_measures()) {
			ticker.ticker_thread
			    = new std::thread(blocking_ticker, std::ref(tickers_on));
		}
	}
}

void metro_private::MetronomePrivate::loop() { engine.eventloop(); }

void metro_private::MetronomePrivate::stop()
{
	tickers_on = false;
	for (auto& [key, ticker] : tickers) {
		if (ticker.ticker_thread->joinable())
			ticker.ticker_thread->join();
	}
}

metro_private::MetronomePrivate::~MetronomePrivate()
{
	stop();
	for (auto& [key, ticker] : tickers) {
		delete ticker.stream;
		delete ticker.ticker_thread;
	}
}
