#include <iostream>
#include <vector>
#include <stdint.h>
#include <algorithm>
#include <math.h>

#include "Stopwatch.h"

template<typename T>
class Array
{
public:
  Array(size_t size) : items(size), nOffset(0), nStep(1) {}
  Array(std::vector<T> const& data) : items(data), nOffset(0), nStep(1) {}

  void setLayer(size_t nOffset, size_t nStep)
  {
    this->nOffset = nOffset;
    this->nStep   = nStep;
  }

  size_t size() const {
    if (nOffset >= items.size())
      return 0;
    size_t length = items.size() - nOffset;
    return length / nStep + (length % nStep ? 1 : 0);
  }

  T& operator[](size_t i)
  {
    return items[nOffset + i * nStep];
  }

  std::vector<T>& data() { return items; }

private:
  std::vector<T> items;
  size_t nOffset = 0;
  size_t nStep   = 1;
};


template<typename T>
void insert_sort(Array<T>& array)
{
  size_t nLength = array.size();
  for (size_t i = 1; i < nLength; ++i) {
    for (size_t j = i; j > 0; --j) {
      if (array[j] < array[j - 1])
        std::swap(array[j], array[j - 1]);
    }
  }
}

template<typename T>
void shell_sort(Array<T>& array, std::vector<uint32_t> const& ks)
{
  for (uint32_t k : ks) {
    for(size_t nOffset = 0; nOffset < k; ++nOffset) {
      array.setLayer(nOffset, k);
      insert_sort(array);
    }
  }
}

std::vector<uint32_t> ShellsCoefficients(uint32_t N)
{
  std::vector<uint32_t> ks;
  uint32_t k = N / 2;
  while(k >= 1) {
    ks.push_back(k);
    k >>= 1;
  }
  return ks;
}

std::vector<uint32_t> HibbardCoefficients(uint32_t N)
{
  std::vector<uint32_t> ks;
  uint32_t k = 2;
  while(k < N) {
    ks.push_back(k - 1);
    k <<= 1;
  }
  std::reverse(ks.begin(), ks.end());
  return ks;
}

std::vector<uint32_t> SedgewickCoefficients(uint32_t N)
{
  std::vector<uint32_t> ks;
  ks.push_back(1);
  uint32_t i = 1;
  while(true) {
    uint32_t k = pow(4, i) + 3 * pow(2, i - 1) + 1;
    if (k > N)
      break;
    ks.push_back(k);
    ++i;
  }
  std::reverse(ks.begin(), ks.end());
  return ks;
}

bool TestInsertSort()
{
  Array<uint32_t> array(1000);
  for (size_t i = 0; i < array.size(); ++i)
    array[i] = i;

  for (size_t i = 0; i < 10; ++i) {
    std::random_shuffle(array.data().begin(), array.data().end());
    insert_sort(array);
    for(size_t j = 0; j < array.size(); ++j)
      if (array[j] != j)
        return false;
  }
  return true;
}

bool TestShellSort()
{
  Array<uint32_t> array(1000);
  for (size_t i = 0; i < array.size(); ++i)
    array[i] = i;

  std::vector<uint32_t> const& ks = ShellsCoefficients(array.size());

  for (size_t i = 0; i < 10; ++i) {
    std::random_shuffle(array.data().begin(), array.data().end());
    shell_sort(array, ks);
    for(size_t j = 0; j < array.size(); ++j)
      if (array[j] != j)
        return false;
  }
  return true;
}

int main(int argc, char* argv[])
{
  std::srand(time(0));

  if (argc == 1) {
    std::cout << "Testing insert_sort: " << (TestInsertSort() ? "OK" : "FAILED") << std::endl;
    std::cout << "Testing shell_sort: "  << (TestShellSort() ? "OK" : "FAILED") << std::endl;
    return 0;
  }

  if (argc != 2)
    return 1;

  uint32_t nLength = atoi(argv[1]);
  std::vector<uint32_t> array(nLength);
  for(uint32_t i = 0; i < nLength; ++i)
    array[i] = i;
  std::random_shuffle(array.begin(), array.end());


  {
    Array<uint32_t> unsortedData(array);
    Stopwatch stopwatch;
    stopwatch.start();
    insert_sort(unsortedData);
    uint32_t nDuration = stopwatch.sinceStartMs();
    std::cout << "insert_sort: " << nDuration << " ms" << std::endl;
  }

  {
    std::vector<uint32_t> const& coefficients = ShellsCoefficients(array.size());
    Array<uint32_t> unsortedData(array);
    Stopwatch stopwatch;
    stopwatch.start();
    shell_sort(unsortedData, coefficients);
    uint32_t nDuration = stopwatch.sinceStartMs();
    std::cout << "shell_sort (Shells): " << nDuration << " ms" << std::endl;
  }

  {
    std::vector<uint32_t> const& coefficients = HibbardCoefficients(array.size());
    Array<uint32_t> unsortedData(array);
    Stopwatch stopwatch;
    stopwatch.start();
    shell_sort(unsortedData, coefficients);
    uint32_t nDuration = stopwatch.sinceStartMs();
    std::cout << "shell_sort (Hibbard): " << nDuration << " ms" << std::endl;
  }

  {
    std::vector<uint32_t> const& coefficients = SedgewickCoefficients(array.size());
    Array<uint32_t> unsortedData(array);
    Stopwatch stopwatch;
    stopwatch.start();
    shell_sort(unsortedData, coefficients);
    uint32_t nDuration = stopwatch.sinceStartMs();
    std::cout << "shell_sort (Sedgewick): " << nDuration << " ms" << std::endl;
  }
  return 0;
}
