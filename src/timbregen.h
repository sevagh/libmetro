#ifndef LIBMETRO_PRIVATE_H
#define LIBMETRO_PRIVATE_H

#include "libmetro.h"

#ifdef UNIT_TESTS
#include <gtest/gtest_prod.h>
#endif /* UNIT_TESTS */

namespace metro_private {
class TimbreGen {
public:
	TimbreGen(metro::Timbre timbre, float level, float frequency, float phase);
	float next_sample(size_t time_point);

private:
	metro::Timbre timbre;
	float level;
	float frequency;
	float phase;

	float sine_sample(size_t t);
	float square_sample(size_t t);
	float sawtooth_sample(size_t t);
	float triangle_sample(size_t t);
};
}; // namespace metro_private

#endif /* LIBMETRO_PRIVATE_H */
