#include "libmetro.h"
#include <vector>

size_t metro::Measure::size() { return notes.size(); }

metro::Measure::Measure(int num_notes)
    : muted(false)
    , notes(num_notes)
{
}

void metro::Measure::toggle_mute(bool mute) { muted = mute; }

bool metro::Measure::is_muted() { return muted; }

metro::Note& metro::Measure::operator[](size_t index) { return notes[index]; }

std::vector<metro::Note>& metro::Measure::get_notes() { return notes; }

const metro::Note& metro::Measure::operator[](size_t index) const
{
	return notes[index];
}
