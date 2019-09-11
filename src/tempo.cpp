#include "rhythm_n_beats.h"
#include <sstream>
#include <string>

std::string
rnb::hello_world()
{
	std::ostringstream ret;
	ret << "Hello, world!";
	return ret.str();
}
