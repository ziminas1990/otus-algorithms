#pragma once

#include <chrono>
#include <vector>
#include <algorithm>
#include <functional>

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


// Creates array, that shuflled completely (nChunkSize == nTotal) or partially
// (nChunkSize < nTotal)
// If nChunkSize has defaule value 0, it will be equivalent to nChunkSize = nTotal
// Examples:
// Not shuffled array:  1 2 3 4 5 6 7 8 9
// nChunkSize = 1:  1 2 3 4 5 6 7 8 9
// nChunkSize = 2:  2 1 - 3 4 - 6 5 - 7 8 - 9
// nChunkSize = 3:  1 3 2 - 5 4 6 - 8 9 7
// nChunkSize = 5:  3 1 4 5 2 - 6 9 7 8
// nChunkSize = 8:  3 1 2 5 4 7 6 8 - 9
// nChunkSize = 9:  7 3 8 1 2 4 9 5 6

template<typename T>
std::vector<T> makeArray(size_t nTotal, uint16_t nChunkSize = 0)
{
  if (nChunkSize == 0)
    nChunkSize = nTotal;
  if (nChunkSize > nTotal)
    nChunkSize = nTotal;


  std::vector<T> elements(nTotal);
  for(size_t i = 0; i < nTotal; ++i)
    elements[i] = i;

  size_t nLeft  = 0;
  size_t nRight = nChunkSize;
  while (nLeft < nTotal) {
    std::random_shuffle(elements.data() + nLeft, elements.data() + nRight - 1);
    nLeft   = nRight;
    nRight += nChunkSize;
    if (nRight > nTotal)
      nRight = nTotal;
  }

  return elements;
}

// Runs some test fTest, that use random numbers generator
// If once test fails, this function prints initial value, that should be used to
// reproduce this problem
std::string RunRandomizedTest(
    std::function<bool()> fTest, unsigned int nInitialRandomValue, size_t nTimes)
{
  unsigned int nRand = nInitialRandomValue % 100000;
  while (nTimes--) {
    std::srand(nRand);
    if (!fTest()) {
      return std::string("FAILED on nRand = ") + std::to_string(nRand);
    }
    nRand = std::rand() % 100000;
  }
  return "OK";
}
