#pragma once
#include <chrono>

class AppTimer
{
public:
	AppTimer() noexcept;
	float GetDeltaTime() noexcept;
	float Peek() const noexcept;
private:
	std::chrono::steady_clock::time_point last;
};