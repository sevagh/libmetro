#include "audioengine.h"
#include "outstream.h"
#include <chrono>
#include <iostream>
#include <soundio/soundio.h>
#include <string>

metro_private::AudioEngine::AudioEngine()
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

void metro_private::AudioEngine::eventloop()
{
	for (;;)
		soundio_wait_events(soundio);
}

metro_private::AudioEngine::~AudioEngine()
{
	soundio_device_unref(device);
	soundio_destroy(soundio);
}

metro_private::OutStream metro_private::AudioEngine::new_outstream(
    std::chrono::microseconds ticker_period)
{
	return metro_private::OutStream(this, ticker_period);
}
