#include "libmetro.h"
#include "libmetro_private.h"
#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <soundio/soundio.h>

TEST(GlobalNamespaceUnitTest, SampleRate)
{
	EXPECT_EQ(metro::SampleRateHz, 48000.0);
}

TEST(NoteUnitTest, EmptyNotes)
{
	auto note1 = metro::Note();
	metro::Note note2;

	EXPECT_EQ(note1.get_frames().size(), 2 * metro::SampleRateHz);
	EXPECT_EQ(note2.get_frames().size(), 2 * metro::SampleRateHz);

	for (size_t i = 0; i < 2 * metro::SampleRateHz; ++i) {
		EXPECT_EQ(note1[i], 0.0);
		EXPECT_EQ(note2[i], 0.0);
	}
}

TEST(NoteUnitTest, SineTimbreVolumeAndAmplitudes)
{
	auto note1 = metro::Note(metro::Timbre::Sine, 440.0, 100.0);
	auto note2 = metro::Note(metro::Timbre::Sine, 440.0, 50.0);

	EXPECT_EQ(note1.get_frames().size(), 2 * metro::SampleRateHz);
	EXPECT_EQ(note2.get_frames().size(), 2 * metro::SampleRateHz);

	auto note1_frames = note1.get_frames();
	auto note2_frames = note2.get_frames();

	auto note1_min = *std::min_element(
	    note1_frames.begin(), note1_frames.begin() + note1_frames.size());
	auto note1_max = *std::max_element(
	    note1_frames.begin(), note1_frames.begin() + note1_frames.size());

	auto note2_min = *std::min_element(
	    note2_frames.begin(), note2_frames.begin() + note2_frames.size());
	auto note2_max = *std::max_element(
	    note2_frames.begin(), note2_frames.begin() + note2_frames.size());

	// check if a 50% volume 440hz sine wave has approx half the peak amplitude
	// of a 100% volume also check 1.0 normalization
	EXPECT_NEAR(note1_min, -1.0, 0.01);
	EXPECT_NEAR(note1_max, 1.0, 0.01);
	EXPECT_NEAR(note2_min, -0.5, 0.01);
	EXPECT_NEAR(note2_max, 0.5, 0.01);
}

TEST(NoteUnitTest, DrumTimbreVolumeAndAmplitudes)
{
	auto note1 = metro::Note(metro::Timbre::Drum, 38.0, 100.0);
	auto note2 = metro::Note(metro::Timbre::Drum, 42.0, 50.0);

	EXPECT_EQ(note1.get_frames().size(), 2 * metro::SampleRateHz);
	EXPECT_EQ(note2.get_frames().size(), 2 * metro::SampleRateHz);

	auto note1_frames = note1.get_frames();
	auto note2_frames = note2.get_frames();

	auto note1_min = *std::min_element(
	    note1_frames.begin(), note1_frames.begin() + note1_frames.size());
	auto note1_max = *std::max_element(
	    note1_frames.begin(), note1_frames.begin() + note1_frames.size());

	auto note2_min = *std::min_element(
	    note2_frames.begin(), note2_frames.begin() + note2_frames.size());
	auto note2_max = *std::max_element(
	    note2_frames.begin(), note2_frames.begin() + note2_frames.size());

	EXPECT_TRUE(note1_min >= -1.0);
	EXPECT_TRUE(note1_max <= 1.0);
	EXPECT_TRUE(note2_min >= -0.5);
	EXPECT_TRUE(note2_max <= 0.5);
}

TEST(NoteUnitTest, IndexOperatorsSpotCheck)
{
	auto note1 = metro::Note(metro::Timbre::Drum, 38.0, 100.0);
	auto note2 = metro::Note(metro::Timbre::Sine, 440.0, 50.0);
	metro::Note note3;

	EXPECT_EQ(note1[36], note1.get_frames()[36]);
	EXPECT_EQ(note2[36], note2.get_frames()[36]);
	EXPECT_EQ(note3[36], note3.get_frames()[36]);

	note1[555] = 1337.0;
	note2[311] = 1234.0;

	EXPECT_EQ(note1.get_frames()[555], 1337.0);
	EXPECT_EQ(note2[311], 1234.0);
}

namespace metro_private {
class SoundIoUnitTest : public ::testing::Test {
protected:
	std::chrono::microseconds period_us = std::chrono::microseconds(5000);
	metro_private::AudioEngine engine;
	metro_private::AudioEngine::OutStream stream
	    = engine.new_outstream(period_us);
};

TEST_F(SoundIoUnitTest, AudioEngineOutputDevice)
{
	// audio output device
	EXPECT_EQ(engine.device->aim, SoundIoDeviceAimOutput);
}

TEST_F(SoundIoUnitTest, AudioEngineOutputDualChannel)
{
	// audio output device
	EXPECT_EQ(engine.device->current_layout.channel_count, 2);
}

TEST_F(SoundIoUnitTest, OutStreamCorrectLatency)
{
	float expected_latency = (period_us.count() / 2.0) / 1000000.0;
	// appropriate software latency for ticker period
	EXPECT_NEAR(stream.outstream->software_latency, expected_latency, 0.01);
}

TEST_F(SoundIoUnitTest, OutStreamCorrectSampleRate)
{
	// appropriate sample rate
	EXPECT_NEAR(stream.outstream->sample_rate, metro::SampleRateHz, 0.01);
}
}; // namespace metro_private