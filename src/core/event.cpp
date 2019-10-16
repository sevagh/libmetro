#include "libjungle/libjungle.h"
#include <thread>
#include <vector>

jungle::core::event::EventCycle::EventCycle(
    std::vector<jungle::core::event::EventFunc> events)
    : events(events)
    , metas(std::map<int, jungle::core::event::EventFunc>{})
    , cycles(0)
    , index(0){};

void jungle::core::event::EventCycle::dispatch_next_event()
{
	auto ret = events.at(index);
	index++;

	if (index >= events.size() - 1) {
		index %= events.size(); // wraparound
		cycles++;
	}

	if (metas.find(cycles) != metas.end()) {
		metas[cycles]();
		cycles = 0;
	}
	std::thread(ret).detach();
}

void jungle::core::event::EventCycle::schedule_meta_event(
    jungle::core::event::EventFunc meta_event,
    int elapsed_cycles)
{
	metas[elapsed_cycles] = meta_event;
}
