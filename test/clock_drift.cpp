#include "libjungle.h"
#include <chrono>
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
#include <thread>

class TempoTest : public testing::TestWithParam<int> {
};

TEST_P(TempoTest, ClockDriftSingleEvents)
{
	int bpm = GetParam();

	std::vector<std::chrono::microseconds> times;

	auto tempo = jungle::tempo::Tempo(bpm);
	double tolerance = 5.0 / 100.0; //%

	std::cout << "Testing if tick drift is within " << tolerance << " for "
	          << bpm << " bpm ticker, for 1 minute worth of ticks" << std::endl;

	jungle::EventCycle record_time
	    = jungle::EventCycle(std::vector<jungle::EventFunc>({
	        [&]() {
		        std::cout << "TICK!" << std::endl;
		        times.push_back(
		            std::chrono::duration_cast<std::chrono::microseconds>(
		                std::chrono::system_clock::now().time_since_epoch()));
	        },
	    }));

	tempo.register_event_cycle(record_time);
	tempo.start();

	// run the test for as many beats as makes 1 minute
	std::this_thread::sleep_for(std::chrono::minutes(1));

	double expected_delta
	    = std::chrono::duration_cast<std::chrono::duration<double>>(
	          tempo.period_us)
	          .count(); // amount of time we expect to elapse between events
	tolerance *= expected_delta;

	for (size_t i = 1; i < times.size(); ++i) {
		double delta
		    = std::chrono::duration_cast<std::chrono::duration<double>>(
		          times[i] - times[i - 1])
		          .count();
		EXPECT_NEAR(delta, expected_delta, tolerance);
	}
}

// INSTANTIATE_TEST_CASE_P(TempoTest, TempoTest, ::testing::Values(10, 200,
// 400));
INSTANTIATE_TEST_CASE_P(TempoTest, TempoTest, ::testing::Values(200, 400));
