
#pragma once

#include <chrono>

class Stopwatch
{
public:
  void start() { startedAt = std::chrono::high_resolution_clock::now(); }

  long sinceStartMs() const
  {
    return
        std::chrono::duration_cast<std::chrono::milliseconds>(
          std::chrono::high_resolution_clock::now() - startedAt).count();
  }

private:
  std::chrono::high_resolution_clock::time_point startedAt;
};
