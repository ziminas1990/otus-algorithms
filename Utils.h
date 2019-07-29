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


// Creates array, that shuflled completely (nShuffledChunks == 1) or partially
// (nShuffledChunks > 1)
// If nShuffledChunks has defaule value 0, it will be equivalent to nShuffledChunks = 1
// Examples:
// Not shuffled array:  1 2 3 4 5 6 7 8 9
// nShuffledChunks = 1:  7 3 8 1 2 4 9 5 6
// nShuffledChunks = 1:  2 3 1 4 - 7 6 8 5 9
// nShuffledChunks = 3:  1 3 2 - 5 4 6 - 8 9 7
// nShuffledChunks = 5:  2 1 - 3 4 - 6 5 - 7 8 - 9
// nShuffledChunks > 5:  1 - 2 - 3 - 4 - 5 - 6 - 7 - 8 - 9
template<typename T>
std::vector<T> makeArray(size_t nTotal, uint16_t nShuffledChunks = 1)
{
  std::vector<T> elements(nTotal);
  for(size_t i = 0; i < nTotal; ++i)
    elements[i] = i;

  if (nShuffledChunks == 0)
    nShuffledChunks = 1;
  if (nShuffledChunks > nTotal)
    nShuffledChunks = nTotal;

  size_t nChunkSize = nTotal / nShuffledChunks;
  for(size_t i = 0; i < nShuffledChunks; ++i) {
    size_t nLeft  = i * nChunkSize;
    size_t nRight = nLeft + nChunkSize;
    if (i == nShuffledChunks - 1)
      nRight = nTotal;  // last block could be bigger than previous
    std::random_shuffle(elements.data() + nLeft, elements.data() + nRight);
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
