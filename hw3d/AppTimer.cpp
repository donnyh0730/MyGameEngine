#include "AppTimer.h"

using namespace std::chrono;

AppTimer::AppTimer() noexcept
{
	last = steady_clock::now();
}

float AppTimer::GetDeltaTime() noexcept
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> frameTime = last - old;
	return frameTime.count();
}

float AppTimer::Peek() const noexcept
{
	return duration<float>( steady_clock::now() - last ).count();
}
