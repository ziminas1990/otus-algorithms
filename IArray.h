#pragma once
#include <stdlib.h>

template<typename T>
class IArray
{
public:
  virtual ~IArray() = default;

  virtual size_t size() const              = 0;
  virtual void   add(T item)               = 0;
  virtual void   add(T item, size_t index) = 0;
  virtual T      get(size_t index) const   = 0;
  virtual T      remove(size_t index)      = 0;

  bool empty()      const { return size() == 0; }
  T    last()       const { return get(size() - 1); }
  T    removeLast()       { return remove(size() - 1); }
};
