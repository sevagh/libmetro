#include "libjungle/libjungle.h"
#include <cassert>
#include <chrono>
#include <soundio/soundio.h>

jungle::core::audio::Engine::Engine()
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
}

static float pick_best_latency(std::chrono::microseconds ticker_period)
{
	return (ticker_period.count() / 2.0) / 1000000.0;
}

jungle::core::audio::Engine::OutStream
jungle::core::audio::Engine::new_outstream(std::chrono::microseconds ticker_period)
{
	float best_latency_s = pick_best_latency(ticker_period);
	return jungle::core::audio::Engine::OutStream(this, best_latency_s);
}

jungle::core::audio::Engine::~Engine()
{
	soundio_device_unref(device);
	soundio_destroy(soundio);
}

void jungle::core::audio::Engine::eventloop()
{
	for (;;)
		soundio_wait_events(soundio);
}
