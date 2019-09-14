#ifndef JUNGLE_H
#define JUNGLE_H

#include <memory>

namespace jungle
{
namespace tempo
{
class Tempo
{
  public:
	Tempo(int bpm);
	~Tempo();
	void
	start();
	void
	stop();
	void
	reset();

  private:
	class impl;
	std::unique_ptr<impl> pimpl;
};
}; // namespace tempo
}; // namespace jungle

#endif /* JUNGLE_H */
