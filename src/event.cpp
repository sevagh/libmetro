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
