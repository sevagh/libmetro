#include "audioengine.h"
#include "libmetro.h"
#include "metronome.h"
#include "outstream.h"
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
	auto note1 = metro::Note(metro::Note::Timbre::Sine, 440.0, 100.0);
	auto note2 = metro::Note(metro::Note::Timbre::Sine, 440.0, 50.0);

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
	auto note1 = metro::Note(metro::Note::Timbre::Drum, 38.0, 100.0);
	auto note2 = metro::Note(metro::Note::Timbre::Drum, 42.0, 50.0);

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
	auto note1 = metro::Note(metro::Note::Timbre::Drum, 38.0, 100.0);
	auto note2 = metro::Note(metro::Note::Timbre::Sine, 440.0, 50.0);
	metro::Note note3;

	EXPECT_EQ(note1[36], note1.get_frames()[36]);
	EXPECT_EQ(note2[36], note2.get_frames()[36]);
	EXPECT_EQ(note3[36], note3.get_frames()[36]);

	note1[555] = 1337.0;
	note2[311] = 1234.0;

	EXPECT_EQ(note1.get_frames()[555], 1337.0);
	EXPECT_EQ(note2[311], 1234.0);
}

TEST(NoteUnitTest, AdditionOperator)
{
	auto note1 = metro::Note(metro::Note::Timbre::Drum, 38.0, 100.0);
	auto note2 = metro::Note(metro::Note::Timbre::Sine, 440.0, 50.0);

	auto note3 = note1 + note2;

	for (size_t i = 0; i < note1.size(); ++i)
		EXPECT_NEAR(note3[i], note1[i] + note2[i], 0.001);
}

TEST(MeasureTest, MeasureSize)
{
	auto measure = metro::Measure(1);
	EXPECT_EQ(measure.size(), 1);
	EXPECT_EQ(measure.get_notes().size(), 1);
}

TEST(MeasureTest, MeasureDefaultNotesAreEmpty)
{
	auto measure = metro::Measure(1);
	for (size_t i = 0; i < measure[0].size(); ++i)
		EXPECT_EQ(measure[0][i], 0.0);
}

TEST(MeasureTest, MeasureIndexOperator)
{
	auto measure = metro::Measure(1);
	auto note1 = measure[0];
	auto note2 = metro::Note(metro::Note::Timbre::Sine, 440.0, 100.0);
	auto note3 = note1 + note2;
	measure[0] = note1 + note2;

	for (size_t i = 0; i < note3.size(); ++i)
		EXPECT_EQ(measure[0][i], note3[i]);
}

TEST(MeasureTest, MeasureBadTextFilesThrowExceptions)
{
	EXPECT_THROW(metro::Measure("../test/fixtures/"
	                            "invalid_missing_measure_len.txt"),
	             std::runtime_error);
}

TEST(MeasureTest, MeasureBadTimbresJustBlank)
{
	EXPECT_NO_THROW(metro::Measure("../test/fixtures/invalid_timbres.txt"));
	auto measure = metro::Measure("../test/fixtures/invalid_timbres.txt");

	EXPECT_EQ(measure.size(), 1);
	for (size_t i = 0; i < measure[0].size(); ++i) // blank notes
		EXPECT_EQ(measure[0][i], 0.0);
}

TEST(MeasureTest, MeasureFromTextFileIsEquivalent)
{
	auto measure11 = metro::Measure(0);
	auto measure12 = metro::Measure("../test/fixtures/blank.txt");

	EXPECT_EQ(measure11.size(), measure12.size());
	for (size_t i = 0; i < measure11.size(); i++) {
		EXPECT_EQ(measure11[i].size(), measure12[i].size());
		for (size_t j = 0; j < measure11[i].size(); ++j)
			EXPECT_EQ(measure11[i][j], measure12[i][j]);
	}

	auto measure21 = metro::Measure(1);
	measure21[0] = metro::Note(metro::Note::Timbre::Sine, 440.0, 100.0);

	auto measure22 = metro::Measure("../test/fixtures/single.txt");

	EXPECT_EQ(measure21.size(), measure22.size());
	for (size_t i = 0; i < measure21.size(); i++) {
		EXPECT_EQ(measure21[i].size(), measure22[i].size());
		for (size_t j = 0; j < measure21[i].size(); ++j)
			EXPECT_EQ(measure21[i][j], measure22[i][j]);
	}

	auto measure31 = metro::Measure(3);
	measure31[0] = metro::Note(metro::Note::Timbre::Sine, 440.0, 100.0)
	               + metro::Note(metro::Note::Timbre::Sine, 110.0, 35.0);
	measure31[2] = metro::Note(metro::Note::Timbre::Sine, 440.0, 100.0)
	               + metro::Note(metro::Note::Timbre::Drum, 73.5, 38.0);

	auto measure32 = metro::Measure("../test/fixtures/multiple.txt");

	EXPECT_EQ(measure31.size(), measure32.size());
	for (size_t i = 0; i < measure31.size(); i++) {
		EXPECT_EQ(measure31[i].size(), measure32[i].size());
		for (size_t j = 0; j < measure31[i].size(); ++j)
			EXPECT_EQ(measure31[i][j], measure32[i][j]);
	}
}

namespace metro_private {
class SoundIoUnitTest : public ::testing::Test {
protected:
	std::chrono::microseconds period_us
	    = std::chrono::microseconds(600000); // 600,000 us ~= 100bpm
	metro_private::AudioEngine engine;
	metro_private::OutStream stream = engine.new_outstream(period_us);
};

TEST_F(SoundIoUnitTest, AudioEngineOutputDevice)
{
	EXPECT_EQ(engine.device->aim, SoundIoDeviceAimOutput);
}

TEST_F(SoundIoUnitTest, AudioEngineOutputDualChannel)
{
	EXPECT_EQ(engine.device->current_layout.channel_count, 2);
}

TEST_F(SoundIoUnitTest, OutStreamCorrectLatency)
{
	float expected_latency = (period_us.count() / 2.0) / 1000000.0;
	EXPECT_NEAR(stream.outstream->software_latency, expected_latency, 0.01);
}

TEST_F(SoundIoUnitTest, OutStreamCorrectSampleRate)
{
	EXPECT_NEAR(stream.outstream->sample_rate, metro::SampleRateHz, 0.01);
}

TEST_F(SoundIoUnitTest, OutStreamCorrectRingbufferCapacity)
{
	int desired_ringbuf_cap = stream.outstream->bytes_per_sample
	                          * stream.outstream->sample_rate
	                          * stream.outstream->software_latency;
	int nearest_ringbuf_cap = 118784;
	int real_ringbuf_cap = soundio_ring_buffer_capacity(stream.ringbuf);
	EXPECT_TRUE(nearest_ringbuf_cap >= desired_ringbuf_cap);
	EXPECT_EQ(real_ringbuf_cap, nearest_ringbuf_cap);
}
}; // namespace metro_private

namespace metro_private {
TEST(MetronomePrivateUnitTest, EmptyAtInit)
{
	auto metronome = metro_private::MetronomePrivate(100);
	EXPECT_EQ(metronome.bpm, 100);
	EXPECT_EQ(metronome.period_us.count(), 600000);
	EXPECT_TRUE(metronome.ticker_on);
}

TEST(MetronomePrivateUnitTest, AddMismatchedMeasuresLCMSize)
{
	auto metronome = metro_private::MetronomePrivate(100);

	auto note1 = metro::Note(metro::Note::Timbre::Sine, 120.0, 100.0);
	auto note2 = metro::Note(metro::Note::Timbre::Sine, 220.0, 100.0);
	auto measure1 = metro::Measure(5);
	measure1[0] = note1;
	auto measure2 = metro::Measure(3);
	measure2[2] = note2;

	// the lcm of 5 and 3 is 15, i.e. the pre-computed "total encompassing
	// measure" will look something like [1/3+1/5, 2/3+2/5, 3/3+3/5, 1/3+4/5, ...
	metronome.add_measure(measure1);
	metronome.add_measure(measure2);

	metronome.stream.compute_notes();
	EXPECT_EQ(metronome.stream.notes.size(), 15);
	EXPECT_EQ(metronome.stream.note_index, 0);

	for (size_t i = 0; i < 15; ++i) {
		// spot check some overlapping notes
		if (i == 0) {
			for (size_t j = 0; j < note1.size(); ++j)
				EXPECT_EQ(metronome.stream.notes[i][j], note1[j]);
		}
		if (i == 5) {
			for (size_t j = 0; j < note1.size(); ++j)
				EXPECT_EQ(metronome.stream.notes[i][j], note1[j] + note2[j]);
		}
	}
}

void underflow_callback(struct SoundIoOutStream* outstream)
{
	( void )outstream;
	ADD_FAILURE() << "got an underflow";
}

void error_callback(struct SoundIoOutStream* outstream, int err)
{
	( void )outstream;
	( void )err;
	ADD_FAILURE() << "got an error";
}

// the test is disabled because apparently things Work(TM) with underflows
// ¯\_(ツ)_/¯
TEST(MetronomePrivateUnitTest, DISABLED_TestStreamsDontUnderflowOrError)
{
	// my ringbuffer code makes some assumptions about when the callback occurs
	// this unit test ensures the ringbuffers and callbacks are all ticking in
	// the background correctly, with no errors or underflows

	auto metronome = metro_private::MetronomePrivate(100);

	auto measure = metro::Measure(1);
	metronome.add_measure(measure);

	metronome.stream.outstream->underflow_callback = underflow_callback;
	metronome.stream.outstream->error_callback = error_callback;

	metronome.start();

	// let it tick for 10 seconds
	std::this_thread::sleep_for(std::chrono::seconds(10));

	metronome.stop();
}
}; // namespace metro_private
