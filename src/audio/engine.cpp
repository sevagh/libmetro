#include "libjungle.h"
#include <algorithm>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <memory>
#include <soundio/soundio.h>
#include <stdio.h>
#include <stk/Stk.h>
#include <vector>

jungle::audio::Engine::Engine()
{
	int err;

	soundio = soundio_create();
	if (!soundio)
		throw std::runtime_error("out of memory");

	if ((err = soundio_connect(soundio)))
		throw std::runtime_error(std::string("error connecting: ")
		                         + soundio_strerror(err));

	soundio_flush_events(soundio);

	int default_out_device_index = soundio_default_output_device_index(soundio);
	if (default_out_device_index < 0)
		throw std::runtime_error("no output device found");

	device = soundio_get_output_device(soundio, default_out_device_index);
	if (!device)
		throw std::runtime_error("out of memory");

	stk::Stk::showWarnings(true);
	stk::Stk::setSampleRate(jungle::SampleRateHz);
}

jungle::audio::Engine::Stream
jungle::audio::Engine::new_stream(std::chrono::microseconds ticker_period)
{
	float best_latency_s = (ticker_period.count() / 2.0) / 1000000.0;
	return jungle::audio::Engine::Stream(this, best_latency_s);
}

jungle::audio::Engine::~Engine()
{
	soundio_device_unref(device);
	soundio_destroy(soundio);
}

void jungle::audio::Engine::eventloop()
{
	for (;;)
		soundio_wait_events(soundio);
}
