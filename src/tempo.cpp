#include "libjungle.h"
#include <chrono>
#include <iomanip>
#include <memory>
#include <stdio.h>
#include <thread>
#include <typeinfo>
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

	period_us = std::chrono::duration_cast<std::chrono::microseconds>(
	    std::chrono::duration<double, std::micro>(1000000.0 * (60.0 / bpm)));
}

void jungle::tempo::Tempo::register_event_cycle(jungle::EventCycle& cycle)
{
	event_cycles.push_back(&cycle);
}

/*
static std::chrono::nanoseconds estimate_cpu_tick_ns()
{
    auto t0 = std::chrono::steady_clock::now();
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0);
}
sleeping for this tiny amount behaves similar to a busy-wait loop, 100% cpu
usage
*/

void jungle::tempo::Tempo::start()
{
	// auto cpu_tick_ns = estimate_cpu_tick_ns();
	// pick a nanosecond sleep somewhere between CPU ticks and desired bpm
	auto tempo_partial_tick_us = period_us / 50.0;

	// std::cout << "cpu_tick_ns: " << cpu_tick_ns.count() << std::endl;
	// std::cout << "tempo_tick_us: " << tempo_tick_ns.count() << std::endl;

	auto blocking_ticker = [&]() {
		auto previous_tick = std::chrono::steady_clock::now();
		while (true) {
			for (auto ec : event_cycles)
				ec->dispatch_next_event();

			while (std::chrono::duration_cast<std::chrono::microseconds>(
			           std::chrono::steady_clock::now() - previous_tick)
			       < period_us) {
				std::this_thread::sleep_for(tempo_partial_tick_us);
			};

			previous_tick = std::chrono::steady_clock::now();
		}
	};

	ticker = std::thread(blocking_ticker);
}

void jungle::tempo::Tempo::stop() { ticker.join(); }

jungle::tempo::Tempo::~Tempo() { stop(); }
