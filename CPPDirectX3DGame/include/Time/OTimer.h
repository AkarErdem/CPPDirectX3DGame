#pragma once
#include <chrono>

class OTimer
{
public:
	OTimer() noexcept;

	/// <summary>
	/// Time elapsed since you last time called Mark
	/// </summary>
	float Mark() noexcept;
	
	/// <summary>
	/// Time elapsed since you last time called Mark without resetting the Mark point
	/// </summary>
	float Peek() const noexcept;

private:
	std::chrono::steady_clock::time_point last;
};

