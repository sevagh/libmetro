#include "audioengine.h"
#include "libmetro.h"
#include "metronome.h"
#include "outstream.h"
#include <chrono>
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

class BpmTimerTestClockAccuracy : public testing::TestWithParam<int> {
};
class BpmTimerTestListen : public testing::TestWithParam<int> {
};

TEST_P(BpmTimerTestClockAccuracy, ClockAccuracy)
{
	int bpm = GetParam();

	std::vector<std::chrono::microseconds> times;

	auto period_us = std::chrono::duration_cast<std::chrono::microseconds>(
	    std::chrono::duration<double, std::micro>(
	        1000000.0 * (60.0 / bpm))); // uses the same computation as bpm ->
	                                    // quarter-note period ticker of the
	                                    // MetronomePrivate object, in
	                                    // src/metronome.cpp

	double tolerance = 2.0; //%

	std::cout << "Testing if ticks are accurate within " << tolerance
	          << "% for " << bpm << " bpm ticker, 20 ticks" << std::endl;

	double expected_delta
	    = std::chrono::duration_cast<std::chrono::duration<double>>(period_us)
	          .count(); // amount of time we expect to elapse between events
	tolerance *= expected_delta;

	for (size_t i = 0; i < 20; ++i) {
		std::thread([&]() {
			times.push_back(
			    std::chrono::duration_cast<std::chrono::microseconds>(
			        std::chrono::system_clock::now().time_since_epoch()));

			if (times.size() > 1) {
				double delta
				    = std::chrono::duration_cast<std::chrono::duration<double>>(
				          times.end()[-1] - times.end()[-2])
				          .count();
				ASSERT_NEAR(delta, expected_delta, tolerance);
			}
		}).detach();

		metro_private::precise_sleep_us(period_us);
	}
}

TEST_P(BpmTimerTestListen, ListenTestNaiveSleepVsPreciseSleep)
{
	int bpm = GetParam();

	auto period_us = std::chrono::duration_cast<std::chrono::microseconds>(
	    std::chrono::duration<double, std::micro>(
	        1000000.0 * (60.0 / bpm))); // uses the same computation as bpm ->
	                                    // quarter-note period ticker of the
	                                    // MetronomePrivate object, in
	                                    // src/metronome.cpp

	auto engine = metro_private::AudioEngine();
	auto outstream1 = engine.new_outstream(period_us);
	auto outstream2 = engine.new_outstream(period_us);

	auto beep1 = metro::Note(metro::Note::Timbre::Sine, 440.0, 100.0);
	auto beep2 = metro::Note(metro::Note::Timbre::Sine, 261.63, 100.0);

	auto measure1 = metro::Measure(3);
	measure1[0] = beep1;

	auto measure2 = metro::Measure(3);
	measure2[0] = beep2;

	outstream1->add_measure(measure1);
	outstream2->add_measure(measure2);

	std::cout << "Emitting 100 beeps with a naive sleep (low pitch) and "
	             "precise sleep (high pitch) simultaneously for bpm "
	          << bpm
	          << ", skipping every 3 quarter notes - see if you can hear the "
	             "difference"
	          << std::endl;

	// precise sleep
	std::thread([&]() {
		outstream1->start();
		for (size_t i = 0; i < 100; ++i) {
			std::thread([&]() { outstream1->play_next_note(); }).detach();
			metro_private::precise_sleep_us(period_us);
		}
	}).detach();

	// naive sleep
	std::thread([&]() {
		outstream2->start();
		for (size_t i = 0; i < 100; ++i) {
			std::thread([&]() { outstream2->play_next_note(); }).detach();
			std::this_thread::sleep_for(period_us);
		}
	}).detach();

	std::this_thread::sleep_for(102 * period_us);
}

INSTANTIATE_TEST_CASE_P(BpmTimerTest,
                        BpmTimerTestClockAccuracy,
                        ::testing::Values(10, 200, 400));
INSTANTIATE_TEST_CASE_P(BpmTimerTest,
                        BpmTimerTestListen,
                        ::testing::Values(500, 700, 900));
