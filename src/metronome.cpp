#include "libmetro.h"
#include "libmetro_private.h"
#include <atomic>
#include <cassert>
#include <cfloat>
#include <chrono>
#include <cstring>
#include <mutex>
#include <soundio/soundio.h>
#include <stk/Drummer.h>
#include <stk/SineWave.h>
#include <stk/Stk.h>
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

void metro::Metronome::change_tempo(int new_bpm)
{
	p_impl->change_bpm(new_bpm);
}

void metro::Metronome::start_and_loop() { p_impl->start(); p_impl->loop(); }

void metro::Metronome::start() { p_impl->start(); }

metro::Metronome::~Metronome()
{
	p_impl->stop();
	delete p_impl;
}

static std::once_flag stk_init_flag;

static void stk_init()
{
	std::call_once(stk_init_flag, []() {
		stk::Stk::showWarnings(true);
		stk::Stk::setSampleRate(metro::SampleRateHz);
	});
}

// reversing the freq2midi magic in the stk Drummer code
static float midi2freq(int midi)
{
	return 440.0 * pow(2.0, (float(midi) - 69.0) / 12.0);
}

static void normalize(std::vector<float>& vec, float ratio)
{
	// normalize to 1.0 * volume_pct since libsoundio expects floats
	// between -1.0 and 1.0
	stk::StkFloat max_elem = -DBL_MAX;
	for (size_t i = 0; i < vec.size(); ++i)
		max_elem = std::max(vec[i], max_elem);

	stk::StkFloat min_elem = DBL_MAX;
	for (size_t i = 0; i < vec.size(); ++i)
		min_elem = std::min(vec[i], min_elem);

	max_elem = std::max(std::abs(min_elem), max_elem);

	for (size_t i = 0; i < vec.size(); ++i)
		vec[i] = (ratio / max_elem) * vec[i];
}

metro::Note::Note()
    : frames(std::vector<float>(2 * metro::SampleRateHz)){}; // empty note

metro::Note::Note(metro::Timbre timbre, float frequency, float volume)
    : frames(std::vector<float>(2 * metro::SampleRateHz))
{
	stk_init();

	switch (timbre) {
	case Sine: {
		stk::SineWave sine;
		sine.setFrequency(frequency);

		for (size_t i = 0; i < frames.size(); ++i)
			frames[i] = sine.tick();

		normalize(frames, volume / 100.0);
	} break;
	case Drum: {
		stk::Drummer drummer;
		drummer.noteOn(midi2freq(frequency), volume / 100.0);
		for (size_t i = 0; i < frames.size(); ++i)
			frames[i] = drummer.tick();

		drummer.noteOff(0.0);
	} break;
	};
}

size_t metro::Note::size() { return frames.size(); }

size_t metro::Measure::size() { return notes.size(); }

metro::Measure::Measure(int num_notes)
    : notes(num_notes)
{
}

metro::Note& metro::Measure::operator[](size_t index) { return notes[index]; }

std::vector<metro::Note>& metro::Measure::get_notes() { return notes; }

const metro::Note& metro::Measure::operator[](size_t index) const
{
	return notes[index];
}

float& metro::Note::operator[](size_t index) { return frames[index]; }

metro::Note metro::Note::operator+(const metro::Note& other)
{
	metro::Note ret;
	for (size_t i = 0; i < ret.size(); ++i)
		ret[i] = (*this)[i] + other[i];
	normalize(ret.frames, 1.0);
	return ret;
}

const float& metro::Note::operator[](size_t index) const
{
	return frames[index];
}

std::vector<float>& metro::Note::get_frames() { return frames; }

void metro::precise_sleep_us(std::chrono::microseconds dur_us)
{
	auto start_tick = std::chrono::steady_clock::now();
	while (std::chrono::duration_cast<std::chrono::microseconds>(
	           std::chrono::steady_clock::now() - start_tick)
	       < dur_us)
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
}

void metro_private::MetronomePrivate::recompute_periods()
{
    period_us_2 = std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::duration<double, std::micro>(2.0 * 1000000.0
                                                    * (60.0 / bpm)));
    period_us_4 = std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::duration<double, std::micro>(1000000.0 * (60.0 / bpm)));
    period_us_8 = std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::duration<double, std::micro>(1.0 / 2.0 * 1000000.0
                                                    * (60.0 / bpm)));
    period_us_16 = std::chrono::duration_cast<std::chrono::microseconds>(
          std::chrono::duration<double, std::micro>(1.0 / 4.0 * 1000000.0
                                                    * (60.0 / bpm)));

    stream_2.change_latency(period_us_2);
    stream_4.change_latency(period_us_4);
    stream_8.change_latency(period_us_8);
    stream_16.change_latency(period_us_16);
}

void metro_private::MetronomePrivate::change_bpm(int new_bpm)
{
	bpm = new_bpm;
	recompute_periods();
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

void metro_private::MetronomePrivate::loop()
{
	engine.eventloop();
}

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
