#include "libjungle/libjungle.h"
#include "libjungle/libjungle_synthesis.h"
#include <chrono>
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

class TempoTest : public testing::TestWithParam<int> {
};

TEST_P(TempoTest, ClockAccuracy)
{
	int bpm = GetParam();

	std::vector<std::chrono::microseconds> times;

	auto tempo = jungle::core::tempo::Tempo(bpm);
	double tolerance = 5.0; //%

	std::cout << "Testing if ticks are accurate within " << tolerance
	          << "% for " << bpm << " bpm ticker, 1 minute worth of ticks"
	          << std::endl;

	double expected_delta
	    = std::chrono::duration_cast<std::chrono::duration<double>>(
	          tempo.period_us)
	          .count(); // amount of time we expect to elapse between events
	tolerance *= expected_delta;

	jungle::core::event::EventCycle record_time = jungle::core::event::EventCycle(
	    std::vector<jungle::core::event::EventFunc>({
	        [&]() {
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
	        },
	    }));

	tempo.register_event_cycle(record_time);
	tempo.start();

	// run the test for as many beats as makes 1 minute
	std::this_thread::sleep_for(std::chrono::minutes(1));

	tempo.stop();
}

INSTANTIATE_TEST_CASE_P(TempoTest, TempoTest, ::testing::Values(10, 200, 400));
