#include "libjungle.h"
#include <chrono>
#include <iomanip>
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
}

void jungle::tempo::Tempo::register_event_cycle(jungle::EventCycle& cycle)
{
	event_cycles.push_back(&cycle);
}

void jungle::tempo::Tempo::start()
{
	auto blocking_ticker = [&]() {
		while (true) {
			for (auto ec : event_cycles)
				ec->dispatch_next_event();

			std::this_thread::sleep_for(std::chrono::microseconds(period_us));
		}
	};

	std::thread(blocking_ticker).detach();
}
