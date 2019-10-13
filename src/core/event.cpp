#include "libjungle/libjungle.h"
#include <thread>
#include <vector>

jungle::core::event::EventCycle::EventCycle(
    std::vector<jungle::core::event::EventFunc> events)
    : events(events)
    , index(0){};

void jungle::core::event::EventCycle::dispatch_next_event()
{
	auto ret = events.at(index);
	index = (index + 1) % events.size(); // wraparound cycle
	std::thread(ret).detach();
}
