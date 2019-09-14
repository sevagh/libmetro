#include "libjungle.h"
#include <chrono>
#include <future>
#include <iomanip>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <thread>
#include <vector>

class jungle::tempo::Tempo::impl
{
  public:
	int bpm;
	int period_ms;
	std::vector<void (*)()> funcs;

	impl(int bpm) : bpm(bpm)
	{
		if (!std::chrono::steady_clock::is_steady)
			throw "std::chrono::steady_clock is unsteady on this platform";

		period_ms = 1000.0 * 60.0 / (double)bpm;

		std::cout << std::fixed;
		std::cout << std::setprecision(2);
	};

	void
	register_func(void (*f)())
	{
		funcs.push_back(f);
	}

	void
	start()
	{
		std::cout << "Starting periodic async executor with bpm: " << bpm
		          << " period: " << period_ms << " (ms)" << std::endl;
		auto _ = std::async(std::launch::async, &impl::blocking_ticker, this);

		std::cout << "press ctrl-c to exit" << std::endl;
		getchar();
	}

	void
	blocking_ticker()
	{
		while (true) {
			auto start = std::chrono::steady_clock::now();
			std::this_thread::sleep_for(std::chrono::milliseconds(period_ms));

			for (auto &f : funcs)
				auto _ = std::async(std::launch::async, f);

			auto end = std::chrono::steady_clock::now();
			std::chrono::duration<double, std::milli> diff = end - start;
			auto tick = diff.count();

			std::cout << "tick " << tick << " , drift " << tick - period_ms
			          << "\r";
			std::fflush(stdout);
		}
	}
};

jungle::tempo::Tempo::Tempo(int bpm) : pimpl(std::make_unique<impl>(impl(bpm)))
{
}

jungle::tempo::Tempo::~Tempo() = default;

// void print_hello()
//{
//	std::cout << "hello" << std::endl;
//}

void
jungle::tempo::Tempo::start()
{
	std::cout << "Start" << std::endl;
	// this->pimpl->register_func(print_hello);
	this->pimpl->start();
}

void
jungle::tempo::Tempo::stop()
{
	std::cout << "Stop" << std::endl;
}

void
jungle::tempo::Tempo::reset()
{
	std::cout << "Reset" << std::endl;
}
