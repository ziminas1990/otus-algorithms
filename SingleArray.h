#pragma once

#include "IArray.h"
#include <memory.h>

// T must be only primitive type!
template<typename T>
class SingleArray : public IArray<T>
{
public:

  ~SingleArray() { delete [] pData; }

  size_t size() const override { return nSize; }

  void add(T item) override
  {
    incrementSize();
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

    T* pNewData = new T[nSize + 1];
    if (index)
      memcpy(pNewData, pData, sizeof(T) * index);
    pNewData[index] = item;
    if (index < nSize)
      memcpy(pNewData + index + 1, pData + index, sizeof(T) * (nSize - index));
    ++nSize;
    delete [] pData;
    pData = pNewData;
  }

  T get(size_t index) const override { return pData[index]; }

  T remove(size_t index) override
  {
    if (index >= nSize)
      return T();

    T removedItem = pData[index];
    --nSize;

    if (!nSize) {
      delete [] pData;
      pData = nullptr;
      return removedItem;
    }

    T* pNewData = new T[nSize];
    if (index)
      memcpy(pNewData, pData, sizeof(T) * index);
    if (index != nSize)
      memcpy(pNewData + index, pData + index + 1, sizeof(T) * (nSize - index));

    delete [] pData;
    pData = pNewData;
    return removedItem;
  }

private:
  void incrementSize()
  {
    T* pNewData = new T[nSize + 1];
    memcpy(pNewData, pData, sizeof(T) * nSize);
    delete [] pData;
    pData = pNewData;
    ++nSize;
  }

private:
  T*     pData = nullptr;
  size_t nSize = 0;
};
