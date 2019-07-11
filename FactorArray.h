#pragma once

#include "IArray.h"
#include <memory.h>

// T must be only primitive type!
template<typename T>
class FactorArray : public IArray<T>
{
public:
  FactorArray(size_t nInitialSize, double factor)
    : pData(new T[nInitialSize]), factor(factor), nCapacity(nInitialSize), nSize(0)
  {}

  ~FactorArray() { delete [] pData; }

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
    size_t nNewCapacity = nCapacity * factor;
    T* pNewData = new T[nNewCapacity];
    if (nSize)
      memcpy(pNewData, pData, sizeof(T) * nSize);
    delete [] pData;
    pData     = pNewData;
    nCapacity = nNewCapacity;
  }

private:
  T*     pData     = nullptr;
  double factor    = 1.5;
  size_t nCapacity = 0;
  size_t nSize     = 0;
};
