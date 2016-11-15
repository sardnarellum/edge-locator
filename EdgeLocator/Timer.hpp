#pragma once

#include <chrono>

template<typename TimeT = std::chrono::microseconds,
	typename ClockT = std::chrono::high_resolution_clock>
class Timer {
private:
	std::chrono::time_point<ClockT> _start, _end;
public:
	Timer() { start(); }
	void start() { _start = _end = ClockT::now(); }
	auto stop() { _end = ClockT::now(); return elapsed(); }
	auto elapsed() {
		return std::chrono::duration_cast<TimeT>(_end - _start).count();
	}
};

