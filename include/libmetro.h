#ifndef LIBMETRO_H
#define LIBMETRO_H

#include <chrono>
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
		Drum  /*!< generate a drum sound. Uses stk/Drummer.h */
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
	 * Drum, frequency represents the General MIDI percussion note i.e.
	 * https://en.wikipedia.org/wiki/General_MIDI#Percussion
	 * @param[in] volume: loudness in percent, i.e. 100% is max volume
	 */
	Note(Timbre timbre, float frequency, float volume);

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
	/*! NoteLength enum */
	enum NoteLength {
		Half,           /*!< 1/2 * bpm of quarter note */
		Quarter,        /*!< bpm of quarter note */
		QuarterTriplet, /*!< 3/2 * bpm of quarter note */
		Eighth,         /*!< 2 * bpm of quarter note */
		EighthTriplet,  /*!< 3 * bpm of quarter note */
		Sixteenth       /*!< 4 * bpm of quarter note */
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
 * 3. Add the measures to the metronome with the given
 * metro::Measure::NoteLength
 * 4. Start and loop the metronome object
 *
 * The metronome will take care of emitting the notes on your output device and
 * cycling through the measures at every tick appropriately.
 */
class Metronome {
public:
	//! Metronome constructor.
	/*!
	 * Create a metronome with the given quarter note bpm.
	 *
	 * @param[in] bpm: bpm of the quarter note
	 */
	Metronome(int bpm);
	~Metronome();

	//! add a measure
	/*!
	 * Add a measure to the metronome which will be cycled through at a
	 * tempo set by param metro::Measure::NoteLength, relative to the
	 * quarter note bpm.
	 *
	 * The goal is to be able to overlay different note durations e.g.
	 * (half, eighth), each with a separate outstream with a different
	 * software_latency due to different timing requirements.
	 *
	 * @param[in] note_length: note length of the measure
	 * @param[in] measure: measure
	 */
	void add_measure(Measure::NoteLength note_length, Measure& measure);

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
