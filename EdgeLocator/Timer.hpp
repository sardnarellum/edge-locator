#pragma once

#include <chrono>
#include <ostream>


template<typename TimeT = std::chrono::microseconds,
	typename ClockT = std::chrono::high_resolution_clock>
class Timer {
private:
	typedef std::chrono::time_point<ClockT> tpoint;
	tpoint _start;
public:
	Timer() { start(); }
	void start() { _start = ClockT::now(); }
	auto elapsed() {
		return std::chrono::duration_cast<TimeT>(ClockT::now() - _start).count();
	}

	friend std::ostream& operator<<(std::ostream&o, Timer timer) {
		o << timer.elapsed();
		return o;
	}
};

