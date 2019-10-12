#include "libjungle.h"
#include <atomic>
#include <chrono>
#include <iomanip>
#include <memory>
#include <stdio.h>
#include <thread>
#include <typeinfo>
#include <vector>

jungle::event::EventCycle::EventCycle(std::vector<jungle::event::EventFunc> events)
    : events(events)
    , index(0){};

void jungle::event::EventCycle::dispatch_next_event()
{
	auto ret = events.at(index);
	index = (index + 1) % events.size(); // wraparound cycle
	std::thread(ret).detach();
}

std::chrono::microseconds jungle::tempo::bpm_to_us(int bpm)
{

	return std::chrono::duration_cast<std::chrono::microseconds>(
	    std::chrono::duration<double, std::micro>(1000000.0 * (60.0 / bpm)));
}

int jungle::tempo::us_to_bpm(std::chrono::microseconds us)
{

	return int(1000000.0 * (60.0 / us.count()));
}

void jungle::tempo::precise_sleep_us(std::chrono::microseconds dur_us)
{
	auto start_tick = std::chrono::steady_clock::now();
	while (std::chrono::duration_cast<std::chrono::microseconds>(
	           std::chrono::steady_clock::now() - start_tick)
	       < dur_us)
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
}

jungle::tempo::Tempo::Tempo(int bpm)
    : bpm(bpm)
    , ticker_on({true})
{
	if (!std::chrono::steady_clock::is_steady)
		throw std::runtime_error("std::chrono::steady_clock is unsteady on "
		                         "this platform");

	period_us = jungle::tempo::bpm_to_us(bpm);
}

void jungle::tempo::Tempo::register_event_cycle(jungle::event::EventCycle& cycle)
{
	event_cycles.push_back(&cycle);
}

std::chrono::microseconds jungle::tempo::Tempo::get_period_us()
{
	return period_us;
}

void jungle::tempo::Tempo::set_bpm(int new_bpm) { bpm = new_bpm; }

void jungle::tempo::Tempo::start()
{
	auto blocking_ticker = [&](std::atomic<bool>& on) {
		while (on) {
			for (auto ec : event_cycles)
				ec->dispatch_next_event();

			precise_sleep_us(period_us);
		}
	};

	ticker_thread = std::thread(blocking_ticker, std::ref(ticker_on));
}

void jungle::tempo::Tempo::stop()
{
	ticker_on = false;
	if (ticker_thread.joinable())
		ticker_thread.join();
}

jungle::tempo::Tempo::~Tempo() { stop(); }
