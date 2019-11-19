#ifndef LIBMETRO_H
#define LIBMETRO_H

#include <chrono>
#include <string>
#include <vector>

/** \mainpage
 *
 * \section intro_sec Overview
 *
 * libmetro is a C++ library for developing interesting metronome variants.
 *
 * libmetro is MIT licensed and hosted at https://github.com/sevagh/libmetro
 *
 * Documentation: libmetro.h
 */

// forward declare the private implementation of Metronome
namespace metro_private {
class MetronomePrivate;
};

//! metro namespace contains everything defined in libmetro
namespace metro {

//! Sample rate used throughout libmetro
/*!
 * This is provided for libmetro users to be able to align with libmetro's
 * sample rate (if they need or want to).
 */
const float SampleRateHz = 48000.0;

//! Note class
/*!
 * Notes wrap a vector of floats to represent a sound. Notes are always
 * 2*48,000 = 96,000 samples long (1 second of data per channel, 2 channels).
 *
 * The real-time, high bpm nature of a metronome requires a low latency on
 * the outstream callback to check for new data at high bpms.
 * Notes of different duration (half, quarter, etc.) are played by adjusting
 * the software_latency of the outstream but supplying the same 96,000 sample
 * notes.
 *
 * Excerpt from SoundIo documentation:
 *
 * >Ignoring hardware latency, this is the number of seconds it takes for the
 * last sample in a full buffer to be played.
 *
 * http://libsound.io/doc-2.0.0/structSoundIoOutStream.html#a20aac1422d3cc64b679616bb8447f06d
 */
class Note {
public:
	/*! Timbre enum */
	enum Timbre {
		Sine, /*!< generate a sine wave sound. Uses stk/SineWave.h */
		Drum, /*!< generate a drum sound. Uses stk/Drummer.h */
	};

	//! Empty Note constructor.
	/*!
	 * Empty notes are essential to libmetro to always keep streams warm by
	 * supplying them with 0s even if no audio is being output at a tick.
	 */
	Note()
	    : frames(std::vector<float>(2 * SampleRateHz)){};

	//! Note constructor with timbre.
	/*!
	 * @param[in] timbre: Sine or Drum.
	 * @param[in] frequency: for Sine, frequency represents the frequency. For
	 * Drum, frequency represents the frequency to General MIDI percussion
	 * note number.
	 *
	 * @param[in] volume: loudness in percent, i.e. 100% is max volume
	 */
	Note(Timbre timbre, float frequency, float volume);

	//! Note constructor with convenient string represenation.
	//
	// Parses a string in the format timbre,freq,vol
	// e.g. sine,440.0,50.0
	//
	/*!
	 * @param[in] triplet: string in format 'timbre,freq,vol'
	 */
	Note(std::string triplet);

	//! index into underlying vector of floats.
	float& operator[](size_t index) { return frames[index]; };

	//! index into underlying vector of floats.
	const float& operator[](size_t index) const { return frames[index]; };

	//! get a reference to the underlying vector of floats.
	std::vector<float>& get_frames() { return frames; };

	//! size of underlying vector of floats.
	size_t size() { return frames.size(); };

	//! sum underlying vectors and return new Note
	Note operator+(const Note& other)
	{
		metro::Note ret;
		for (size_t i = 0; i < ret.size(); ++i)
			ret[i] = (*this)[i] + other[i];
		return ret;
	}

private:
	std::vector<float> frames;
};

//! Measure class
/*!
 * Measures wrap a vector of Notes to represent a measure.
 *
 * Unset indices of a measure will contain an empty note. As explained in the
 * Note documentation, this is to keep the real-time audio streams "warm" by
 * always emitting the correct amount of samples to obey timing, even in a
 * metronome tick that doesn't have a sound.
 */
class Measure {
public:
	/*! FileFormat enum */
	enum FileFormat {
		One, /*!< first experimental file format */
		Two, /*!< second experimental file format */
	};

	//! Measure constructor.
	/*!
	 * Assign notes with the index operator e.g.
	 * auto measure = Measure(5); measure[0] = metro::Note();
	 *
	 * Accesses out of range are undefined behavior (but it'll probably
	 * segfault).
	 *
	 * @param[in] num_notes: number of notes (allocates size of underlying
	 * vector)
	 */
	Measure(int num_notes)
	    : notes(num_notes){};

	//! Measure constructor.
	/*!
	 * Create a measure by parsing a measure description txt file.
	 *
	 * **FileFormat::One**
	 *
	 * Lines are newline-separated. Comments start with '#'
	 *
	 * 	# number of beats in the measure
	 * 	measure_length 4
	 *
	 * 	# beats at index of the measure
	 * 	0 sine,440.0,10.0 drum,73.42,100.0 drum,92.5,100.0
	 * 	1 sine,440.0,10.0 drum,73.42,50.0
	 * 	2 sine,440.0,10.0 drum,73.42,65.0
	 * 	3 sine,440.0,10.0 drum,73.42,50.0
	 *
	 * **FileFormat::Two**
	 *
	 * Lines are newline-separated. Comments start with '#'
	 *
	 * 	# every string should have the same length
	 * 	# the first 1 represents a downbeat
	 * 	# subsequent 1s represent weak beats
	 * 	# this is a way to visualize how polyrhythms overlay on one another
	 * 	1 0 1 0 1 0
	 * 	1 0 0 1 0 0
	 *
	 * @param[in] path: path to txt file
	 */
	Measure(const char* path, FileFormat file_format);

	//! index into underlying vector of Notes.
	Note& operator[](size_t index) { return notes[index]; };

	//! index into underlying vector of Notes.
	const Note& operator[](size_t index) const { return notes[index]; };

	//! get a reference to the underlying vector of Notes.
	std::vector<Note>& get_notes() { return notes; };

	//! size of underlying vector of Notes.
	size_t size() { return notes.size(); };

private:
	std::vector<Note> notes;
};

//! Metronome class
/*!
 * Metronome is the main ticker of libmetro.
 *
 * The flow of libmetro goes something like:
 *
 * 1. Create a metronome with the desired quarter note bpm
 * 2. Create measures with notes
 * 3. Add the measures to the metronome
 * 4. Start and loop the metronome object
 *
 * The metronome will take care of emitting the notes on your output device and
 * cycling through the measures at every tick appropriately.
 */
class Metronome {
public:
	//! Metronome constructor.
	/*!
	 * Create a metronome with the given bpm.
	 *
	 * @param[in] bpm: bpm to cycle through the measure
	 */
	Metronome(int bpm);

	~Metronome();

	//! add a measure
	/*!
	 * Add a measure to the metronome which will be cycled through at a
	 * tempo set by the bpm param.
	 *
	 * @param[in] note_length: note length of the measure
	 * @param[in] measure: measure
	 */
	void add_measure(Measure& measure);

	//! start the loop
	/*!
	 * This method requires the user to block afterwards or else the code
	 * will exit. Useful if you want your own custom blocking logic e.g.
	 * waiting for user key presses.
	 */
	void start();

	//! start the loop and block
	void start_and_loop();

private:
	metro_private::MetronomePrivate*
	    p_impl; // https://en.cppreference.com/w/cpp/language/pimpl
};
}; // namespace metro

#endif /* LIBMETRO_H */
