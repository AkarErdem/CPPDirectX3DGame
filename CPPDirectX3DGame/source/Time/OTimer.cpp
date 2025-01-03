#include "Time/OTimer.h"
using namespace std::chrono;

OTimer::OTimer() noexcept
{
	last = steady_clock::now();
}

float OTimer::Mark() noexcept
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float OTimer::Peek() const noexcept
{
	return duration<float>(steady_clock::now() - last).count();
}