#include "libmetro.h"
#include "metronome.h"
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

// measure from file path constructor
metro::Measure::Measure(const char* path)
{
	std::ifstream infile(path);
	std::string line;

	size_t measure_len;

	std::vector<size_t> input_note_indices;
	std::vector<metro::Note> input_notes;

	while (std::getline(infile, line)) {
		if (std::all_of(line.begin(), line.end(), isspace))
			continue; // ignore blank lines
		if (line.find("#") == 0)
			continue; // ignore comments

		std::istringstream iss(line);

		std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
		                                std::istream_iterator<std::string>{}};

		if (tokens.size() < 2)
			continue;

		if (tokens[0].compare("measure_length") == 0) {
			if (tokens.size() > 2)
				throw std::runtime_error("line measure_length can only have 1 "
				                         "field");
			measure_len = std::stoi(tokens[1]);
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

	notes = std::vector<metro::Note>(measure_len);

	for (size_t i = 0; i < input_note_indices.size(); ++i)
		notes[input_note_indices[i]] = input_notes[i];
}
