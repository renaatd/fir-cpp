#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <chrono>

class Stopwatch {
  public:
    int elapsed() {
	auto stop = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(stop - _start).count();
	return elapsed;
    }

  private:
    const std::chrono::time_point<std::chrono::high_resolution_clock> _start { std::chrono::high_resolution_clock::now() };
};

#endif
