#include "metronome.h"
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

void metro::Metronome::add_measure(metro::Measure& measure)
{
	p_impl->add_measure(measure);
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

void metro_private::precise_sleep_us(std::chrono::microseconds dur_us)
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
    , period_us(std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::duration<double, std::micro>(1000000.0 * (60.0 / bpm))))
    , stream(engine.new_outstream(period_us))
    , ticker_on({true})
{
	if (!std::chrono::steady_clock::is_steady)
		throw std::runtime_error("std::chrono::steady_clock is unsteady on "
		                         "this platform");
}

void metro_private::MetronomePrivate::add_measure(metro::Measure& measure)
{
	stream.add_measure(measure);
}

void metro_private::MetronomePrivate::start()
{

	auto blocking_ticker = [&](std::atomic<bool>& on) {
		stream.start();
		while (on) {
			std::thread([&]() { stream.play_next_note(); }).detach();
			metro_private::precise_sleep_us(period_us);
		}
	};

	ticker_thread = std::thread(blocking_ticker, std::ref(ticker_on));
}

void metro_private::MetronomePrivate::loop() { engine.eventloop(); }

void metro_private::MetronomePrivate::stop()
{
	ticker_on = false;
	if (ticker_thread.joinable())
		ticker_thread.join();
}

metro_private::MetronomePrivate::~MetronomePrivate() { stop(); }
