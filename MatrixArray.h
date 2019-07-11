#pragma once

#include "IArray.h"
#include "VectorArray.h"
#include <memory.h>

// T must be only primitive type!
template<typename T>
class MatrixArray : public IArray<T>
{
public:
  MatrixArray(size_t nRowSize,
              IArray<IArray<T>*>* pBaseArray = new VectorArray<IArray<T>*>(10))
    : pRows(pBaseArray), nRowSize(nRowSize)
  {}

  ~MatrixArray()
  {
    for (size_t i = 0; i < pRows->size(); ++i) {
      delete pRows->get(i);
    }
    delete pRows;
  }

  size_t size() const override { return nSize; }

  void add(T item) override
  {
    if (size() == capacity())
      expand();
    IArray<T>* pRow = pRows->get(size() / nRowSize);
    pRow->add(item);
    ++nSize;
  }

  void add(T item, size_t index) override
  {
    if (size() == capacity())
      expand();
    size_t nRowToInsert = index / nRowSize;

    for (size_t nRowId = nTotalRows - 1; nRowId > nRowToInsert; --nRowId) {
      IArray<T>* pThisRow = pRows->get(nRowId);
      IArray<T>* pPrevRow = pRows->get(nRowId - 1);
      pThisRow->add(pPrevRow->removeLast(), 0);
      pThisRow->size();
    }
    pRows->get(nRowToInsert)->add(item, index % nRowSize);
    ++nSize;
  }

  T get(size_t index) const override {
    return pRows->get(index / nRowSize)->get(index % nRowSize);
  }

  T remove(size_t index) override
  {
    if (index >= size())
      return T();

    size_t nRemoveRowId = index / nRowSize;
    T removedElement = pRows->get(nRemoveRowId)->remove(index % nRowSize);

    for (size_t nRowId = nRemoveRowId + 1; nRowId < nTotalRows; ++nRowId) {
      IArray<T>* pThisRow = pRows->get(nRowId);
      if (pThisRow->empty())
        break;
      IArray<T>* pPrevRow = pRows->get(nRowId - 1);
      pPrevRow->add(pThisRow->remove(0));
    }
    --nSize;

    return removedElement;
  }

private:
  void expand()
  {
    IArray<T>* pNewRow = new VectorArray<T>(nRowSize);
    pRows->add(pNewRow);
    ++nTotalRows;
  }

  size_t capacity() const { return nRowSize * nTotalRows; }

private:
  IArray<IArray<T>*>* pRows = nullptr;
  size_t nTotalRows = 0;
  size_t nSize      = 0;
  size_t nRowSize   = 0;
};
