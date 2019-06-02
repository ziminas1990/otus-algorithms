#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <algorithm>
#include <vector>
#include <memory.h>

class Bitarray
{
public:
  Bitarray(size_t nElements)
    : pData(new uint8_t[nElements / 8 + 1]), nElements(nElements)
  {
    memset(pData, 0, sizeof(uint8_t)*(nElements / 8 + 1));
  }
  ~Bitarray() { delete [] pData; }

  void set(size_t nElement)
  {
    if (nElement >= nElements)
      return;
    uint8_t& cell      = pData[nElement / 8];
    size_t   bitOffset = nElement % 8;
    uint8_t  nMask     = 1 << bitOffset;
    if (!(cell & nMask))
      cell += nMask;
  }

  bool test(size_t nElement)
  {
    if (nElement >= nElements)
      return false;
    uint8_t cell      = pData[nElement / 8];
    size_t  bitOffset = nElement % 8;
    uint8_t nMask     = 1 << bitOffset;
    return cell & nMask;
  }

private:
  uint8_t* pData     = nullptr;
  size_t   nElements = 0;
};


bool TestBitArray()
{
  std::vector<size_t> indexes(128);
  for (size_t i = 0; i < indexes.size(); ++i)
    indexes[i] = i;

  Bitarray bits(128);

  for(size_t i = 0; i < 1000; ++i) {
    std::random_shuffle(indexes.begin(), indexes.end());
    // Filling bits array
    for(size_t j = 0; j < indexes.size() / 2; ++j)
      bits.set(j);
    // Checking bits array
    for (size_t j = 0; j < indexes.size(); ++j) {
      bool lExpected = j < indexes.size() / 2;
      if (bits.test(j) != lExpected)
        return false;
    }
  }
  return true;
}
