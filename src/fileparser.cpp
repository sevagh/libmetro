#include "libmetro.h"
#include "metronome.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

static std::vector<std::vector<metro::Note>> beats{
    {metro::Note(metro::Note::Timbre::Drum, 73.42, 100.0)
         + metro::Note(metro::Note::Timbre::Drum, 92.50, 100.0), // downbeat 0
     metro::Note{metro::Note::Timbre::Drum, 73.42, 50.0}},       // weakbeat 0
    {metro::Note(metro::Note::Timbre::Drum, 207.65, 100.0)
         + metro::Note(metro::Note::Timbre::Drum, 185.0, 100.0), // downbeat 1
     metro::Note{metro::Note::Timbre::Drum, 207.65, 50.0}},      // weakbeat 1
};

// measure from file path constructor
metro::Measure::Measure(const char* path,
                        metro::Measure::FileFormat file_format)
{
	std::ifstream infile(path);
	if (infile.fail())
		throw std::runtime_error(std::string("error loading file ") + path);

	std::string line;

	switch (file_format) {
	case metro::Measure::FileFormat::One: {
		size_t measure_len;
		bool measure_len_seen = false;

		std::vector<size_t> input_note_indices;
		std::vector<metro::Note> input_notes;

		while (std::getline(infile, line)) {
			if (std::all_of(line.begin(), line.end(), isspace))
				continue; // ignore blank lines
			if (line.find("#") == 0)
				continue; // ignore comments

			std::istringstream iss(line);

			std::vector<std::string> tokens{
			    std::istream_iterator<std::string>{iss},
			    std::istream_iterator<std::string>{}};

			if (tokens.size() < 2)
				continue;

			if (tokens[0].compare("measure_length") == 0) {
				if (tokens.size() > 2)
					throw std::runtime_error("line measure_length can only "
					                         "have 1 "
					                         "field");
				measure_len = std::stoi(tokens[1]);
				measure_len_seen = true;
				continue;
			}

			try {
				size_t note_idx = std::stol(tokens[0]);

				metro::Note note;

				for (auto it = tokens.begin() + 1; it != tokens.end(); ++it)
					note = note + metro::Note(*it);

				input_notes.push_back(note);
				input_note_indices.push_back(note_idx);
			}
			catch (const std::exception& exc) {
				std::cerr << "line '" << line << "' unrecognized... skipping"
				          << std::endl;
				std::cerr << "exception: " << exc.what() << std::endl;
				continue;
			}
		}

		if (!measure_len_seen)
			throw std::runtime_error("Missing 'measure_length' in text file");

		notes = std::vector<metro::Note>(measure_len);

		for (size_t i = 0; i < input_note_indices.size(); ++i)
			notes[input_note_indices[i]] = input_notes[i];
		break;
	}
	case metro::Measure::FileFormat::Two: {
		size_t measure_len = 0;

		notes = std::vector<metro::Note>();
		size_t line_count = 0;

		while (std::getline(infile, line)) {
			if (std::all_of(line.begin(), line.end(), isspace))
				continue; // ignore blank lines
			if (line.find("#") == 0)
				continue; // ignore comments

			std::istringstream iss(line);

			std::vector<std::string> tokens{
			    std::istream_iterator<std::string>{iss},
			    std::istream_iterator<std::string>{}};

			if (measure_len == 0) {
				measure_len = tokens.size();
				notes.resize(measure_len);
			}
			else if (tokens.size() != measure_len)
				throw std::runtime_error("lines must have same number of "
				                         "tokens");

			try {
				for (size_t i = 0; i < tokens.size(); ++i) {
					if (tokens[i].compare("1") == 0) {
						if (i == 0)
							// first note is a downbeat
							notes[i]
							    = notes[i] + metro::Note(beats[line_count][0]);
						else
							notes[i]
							    = notes[i] + metro::Note(beats[line_count][1]);
					}
				}
				line_count++;
			}
			catch (const std::exception& exc) {
				std::cerr << "line '" << line << "' unrecognized... skipping"
				          << std::endl;
				std::cerr << "exception: " << exc.what() << std::endl;
				continue;
			}
		}
		break;
	}
	};
}
