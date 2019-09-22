#include "libjungle.h"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <thread>
#include <vector>

jungle::EventCycle::EventCycle(std::vector<jungle::EventFunc> events)
    : events(events)
    , index(0){};

void jungle::EventCycle::dispatch_next_event()
{
	auto ret = events.at(index);
	index = (index + 1) % events.size(); // wraparound cycle
	std::thread(ret).detach();
}

jungle::tempo::Tempo::Tempo(int bpm)
    : bpm(bpm)
{
	if (!std::chrono::steady_clock::is_steady)
		throw std::runtime_error("std::chrono::steady_clock is unsteady on "
		                         "this platform");

	period_us = 1000000.0 * (60.0 / bpm);

	std::cout << std::fixed;
	std::cout << std::setprecision(2);
}

void jungle::tempo::Tempo::register_event_cycle(jungle::EventCycle& cycle)
{
	event_cycles.push_back(&cycle);
}

void jungle::tempo::Tempo::start()
{
	std::cout << "Starting periodic async executor with bpm: " << bpm
	          << " period: " << period_us << " us" << std::endl;

	auto blocking_ticker = [&]() {
		while (true) {
			for (auto ec : event_cycles)
				ec->dispatch_next_event();

			auto start = std::chrono::steady_clock::now();
			std::this_thread::sleep_for(std::chrono::microseconds(period_us));
			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double, std::micro> diff = end - start;
			auto drift_pct
			    = ((diff.count() - ( double )period_us) / ( double )period_us)
			      * 100.0;

			std::cout << "clock drift " << drift_pct << "%\r";
			std::fflush(stdout);
		}
	};

	std::thread(blocking_ticker).detach();
}
