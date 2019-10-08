#include "libjungle.h"
#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <cmath>

class TempoTest : public testing::TestWithParam<int> { };

TEST_P(TempoTest, ClockDriftSingleEvents) {
	int bpm = GetParam();

	std::vector<unsigned long long> times;

	auto tempo = jungle::tempo::Tempo(bpm);
	double tolerance = 5.0/100.0; //%

	jungle::EventCycle record_time = jungle::EventCycle(std::vector<jungle::EventFunc>({
	    [&]() {
			times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count());
	    },
	}));

	tempo.register_event_cycle(record_time);
	tempo.start();

	// run the test for as many beats as makes 1 minute
	//std::this_thread::sleep_for(std::chrono::minutes(1));
	std::this_thread::sleep_for(std::chrono::seconds(10));

	double expected_delta = tempo.period_us; //amount of time we expect to elapse between events

	tolerance *= expected_delta;

	for (size_t i = 1; i < times.size(); ++i) {
		double delta = times[i] - times[i-1];
		std::cout << delta << std::endl;
		EXPECT_NEAR(delta, expected_delta, tolerance);
	}
}

INSTANTIATE_TEST_CASE_P(TempoTest, TempoTest,
		    ::testing::Values(10, 200, 400));
