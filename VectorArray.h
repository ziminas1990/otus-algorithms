#pragma once

#include "IArray.h"
#include <memory.h>

// T must be only primitive type!
template<typename T>
class VectorArray : public IArray<T>
{
public:
  VectorArray(size_t delta) : pData(nullptr), nDelta(delta), nCapacity(0), nSize(0) {}

  ~VectorArray() { delete [] pData; }

  size_t size() const override { return nSize; }

  void add(T item) override
  {
    if (nSize == nCapacity)
      expand();
    ++nSize;
    pData[nSize - 1] = item;
  }

  void add(T item, size_t index) override
  {
    if (index > nSize)
      return;
    if (index == nSize) {
      add(item);
      return;
    }
    if (nSize == nCapacity)
      expand();

    size_t nTotalElementsToMove = nSize - index;
    if (nTotalElementsToMove)
      memmove(pData + index + 1, pData + index, sizeof(T) * nTotalElementsToMove);
    pData[index] = item;
    ++nSize;
  }

  T get(size_t index) const override { return pData[index]; }

  T remove(size_t index) override
  {
    if (index >= nSize)
      return T();

    T removedItem = pData[index];

    size_t nTotalElementsToMove = nSize - index - 1;
    if (nTotalElementsToMove)
      memmove(pData + index, pData + index + 1, sizeof(T) * nTotalElementsToMove);

    --nSize;
    return removedItem;
  }

private:
  void expand()
  {
    T* pNewData = new T[nCapacity + nDelta];
    if (nSize)
      memcpy(pNewData, pData, sizeof(T) * nSize);
    delete [] pData;
    pData      = pNewData;
    nCapacity += nDelta;
  }

private:
  T*     pData     = nullptr;
  size_t nDelta    = 10;
  size_t nCapacity = 0;
  size_t nSize     = 0;
};
