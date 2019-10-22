#include "libmetro.h"
#include <gtest/gtest.h>
#include <iostream>

TEST(NotesTest, EmptyNotes)
{
	auto note1 = metro::Note();
	metro::Note note2;

	EXPECT_EQ(note1.get_frames().size(), 2 * metro::SampleRateHz);
	EXPECT_EQ(note2.get_frames().size(), 2 * metro::SampleRateHz);
}
