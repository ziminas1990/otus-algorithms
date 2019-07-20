#include <iostream>
#include <vector>
#include <stdint.h>
#include <algorithm>
#include <math.h>
#include <memory.h>

#include "Stopwatch.h"

template<typename T>
inline void insert_sort(T* array, size_t nLength)
{
  for (size_t i = 1; i < nLength; ++i) {
    for (size_t j = i; j > 0; --j) {
      if (array[j] < array[j - 1])
        std::swap(array[j], array[j - 1]);
      else
        break;
    }
  }
}

template<typename T>
void merge(T const* pBegin, T const* pMiddle, T const* pEnd, T* pOutput)
{
  T const* pLeft  = pBegin;
  T const* pRight = pMiddle;

  while(true) {
    if (pLeft < pMiddle && (pRight >= pEnd || *pLeft < *pRight)) {
      *pOutput++ = *pLeft++;
      continue;
    } else if (pRight < pEnd) {
      *pOutput++ = *pRight++;
      continue;
    }
    break;
  }
}

// sorting array [pBegin, pEnd) into array pArray
template<typename T>
void merge_sort(T* pBegin, T* pEnd, T* pArray)
{
  size_t nLegnth = pEnd - pBegin;
  if (nLegnth <= 4) {
    insert_sort(pArray, nLegnth);
    return;
  }

  size_t nMiddle = nLegnth / 2;
  T*     pMiddle = pBegin + nMiddle;

  merge_sort(pArray,           pArray + nMiddle, pBegin);
  merge_sort(pArray + nMiddle, pArray + nLegnth, pMiddle);

  merge(pBegin, pMiddle, pEnd, pArray);
}

template<typename T>
void merge_sort(T* pArray, size_t nLgenth)
{
  T* pCopy = new T[nLgenth];
  memcpy(pCopy, pArray, nLgenth * sizeof(T));
  merge_sort(pCopy, pCopy + nLgenth, pArray);
  delete [] pCopy;
}


bool checkSortedArray(uint32_t const* pArray, size_t nLength)
{
  for(size_t i = 1; i < nLength; ++i)
    if (pArray[i] < pArray[i - 1])
      return false;
  return true;
}

bool TestMergeSort()
{
  size_t const pLength = 1000;
  uint32_t pArray[pLength];
  for (uint32_t i = 0; i < pLength; ++i)
    pArray[i] = i;

  for (size_t i = 0; i < 10000; ++i) {
    std::random_shuffle(pArray, pArray + pLength);
    merge_sort(pArray, pLength);
    if (!checkSortedArray(pArray, pLength))
      return false;
  }
  return true;
}

int main(int argc, char* argv[])
{
  std::srand(time(nullptr));

  if (argc == 1) {
    std::cout << "Testing insert_sort: " << (TestMergeSort() ? "OK" : "FAILED") << std::endl;
    return 0;
  }

  if (argc != 2)
    return 1;

  uint32_t nLength = atoi(argv[1]);
  uint32_t* pArray = new uint32_t[nLength];
  for(uint32_t i = 0; i < nLength; ++i)
    pArray[i] = i;

  std::random_shuffle(pArray, pArray + nLength);
  Stopwatch stopwatch;
  stopwatch.start();
  merge_sort(pArray, nLength);
  long duration = stopwatch.sinceStartMs();
  std::cout << "Time: " << duration << " ms (" <<
               (checkSortedArray(pArray, nLength) ? "correct" : "incorrect") <<
               ")" << std::endl;
  return 0;
}
