#include "libjungle.h"
#include <chrono>
#include <iomanip>
#include <memory>
#include <stdio.h>
#include <thread>
#include <typeinfo>
#include <vector>
#include <atomic>

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
    : bpm(bpm),
      ticker_on({ true })
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

static std::chrono::nanoseconds estimate_cpu_tick_ns()
{
    auto t0 = std::chrono::steady_clock::now();
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0);
}

void jungle::tempo::Tempo::start()
{
	auto cpu_tick_ns = estimate_cpu_tick_ns();

	std::cout << "cpu_tick  ns: " << cpu_tick_ns.count() << std::endl;
	std::cout << "period    ns: " << std::chrono::duration_cast<std::chrono::nanoseconds>(period_us).count() << std::endl;
	std::cout << "period/10 ns: " << std::chrono::duration_cast<std::chrono::nanoseconds>(period_us/10.0).count() << std::endl;

	auto blocking_ticker = [&](std::atomic<bool>& on) {
		auto previous_tick = std::chrono::steady_clock::now();
		while (on) {
			for (auto ec : event_cycles)
				ec->dispatch_next_event();

			while (std::chrono::duration_cast<std::chrono::microseconds>(
			           std::chrono::steady_clock::now() - previous_tick)
			       < period_us)
				std::this_thread::sleep_for(cpu_tick_ns);

			previous_tick = std::chrono::steady_clock::now();
		}
	};

	std::thread(blocking_ticker, std::ref(ticker_on)).detach();
}

void jungle::tempo::Tempo::stop() { ticker_on = false; }

jungle::tempo::Tempo::~Tempo() { stop(); }
