#include "libjungle/libjungle.h"
#include <thread>
#include <vector>

jungle::core::event::EventCycle::EventCycle(
    std::vector<jungle::core::event::EventFunc> events)
    : events(events)
    , cycle(0)
    , index(0){};

void jungle::core::event::EventCycle::dispatch_next_event()
{
	// schedule the meta event at the beginning of the measure
	// that it was scheduled
	while (next_metas.size() != 0 && next_metas.back() != 0) {
		std::thread(next_metas.back()).detach();
		next_metas.pop_back(); // destroy it after calling it
	}

	auto ret = events.at(index);
	index++;

	if (index > events.size() - 1) {
		index %= events.size();
		cycle++; // new measure

		// schedule meta events
		// e.g. "mute the metronome every 4th measure
		for (auto iter = metas.begin(); iter != metas.end(); ++iter) {
			if (cycle != 0 && cycle % iter->first == 0) {
				next_metas.push_back(iter->second);
			}
		}
	}

	std::thread(ret).detach();
}

void jungle::core::event::EventCycle::schedule_meta_event(
    jungle::core::event::EventFunc meta,
    size_t elapsed_cycles)
{
	metas[elapsed_cycles] = meta;
}
