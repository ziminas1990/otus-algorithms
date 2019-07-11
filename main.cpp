#include "SingleArray.h"
#include "VectorArray.h"
#include "FactorArray.h"
#include "MatrixArray.h"
#include <chrono>
#include <iostream>
#include <cstdlib>

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


bool functionalTests(IArray<size_t>& array)
{
  for (size_t i = 0; i < 9; ++i) {
    array.add(i);
  }

  // Current array state: 0 1 2 3 4 5 6 7 8
  if (array.size() != 9)
    return false;
  for (size_t i = 0; i < 9; ++i)
    if (array.get(i) != i)
      return false;

  array.add(100, 2);
  // Current array state: 0 1 100 2 3 4 5 6 7 8
  if (array.size() != 10 || array.get(2) != 100)
    return false;

  array.add(200, array.size());
  // Current array state: 0 1 100 2 3 4 5 6 7 8 200
  if (array.size() != 11 || array.get(array.size() - 1) != 200)
    return false;

  array.add(300, 0);
  // Current array state: 300 0 1 100 2 3 4 5 6 7 8 200
  if (array.size() != 12 || array.get(0) != 300)
    return false;

  if (array.remove(3) != 100)
    return false;
  if (array.remove(array.size() - 1) != 200)
    return false;
  if (array.remove(0) != 300)
    return false;

  // Current array state: 0 1 2 3 4 5 6 7 8
  if (array.size() != 9)
    return false;
  for (size_t i = 0; i < 9; ++i)
    if (array.get(i) != i)
      return false;

  // Removing all elements and than adding new elements
  while (array.size()) {
    array.remove(0);
  }
  for (size_t i = 0; i < 100; ++i)
    array.add(i);
  if (array.size() != 100)
    return false;
  for (size_t i = 0; i < 100; ++i)
    if (array.get(i) != i)
      return false;
  return true;
}

template<typename ArrayType, typename... Args>
void AppendTest(std::string const& sArrayType, Args... args)
{
  std::cout << sArrayType << ": ";
  for (size_t nTotal : {10000, 100000, 1000000})
  {
    ArrayType array(args...);
    Stopwatch stopwatch;
    stopwatch.start();
    for (size_t i = 0; i < nTotal; ++i) {
      array.add(i);
    }
    std::cout << stopwatch.sinceStartMs() << " ";
  }
  std::cout << std::endl;
}

template<typename T>
void InsertToHeadTest(IArray<T>* pArray, std::string const& sArrayType)
{
  std::cout << sArrayType << ": ";
  Stopwatch stopwatch;
  stopwatch.start();

  for (size_t i = 0; i < 100000; ++i)
    pArray->add(i, 0);
  std::cout << stopwatch.sinceStartMs() << std::endl;
}

template<typename T>
void InsertToTailTest(IArray<T>* pArray, std::string const& sArrayType)
{
  std::cout << sArrayType << ": ";
  Stopwatch stopwatch;
  stopwatch.start();

  for (size_t i = 0; i < 100000; ++i)
    pArray->add(i, pArray->size());
  std::cout << stopwatch.sinceStartMs() << std::endl;
}

template<typename T>
void RandomInsertTest(IArray<T>* pArray, std::string const& sArrayType)
{
  std::cout << sArrayType << ": ";
  Stopwatch stopwatch;
  stopwatch.start();

  std::srand(100);
  for (size_t i = 0; i < 100000; ++i)
  {
    size_t index = pArray->size() * std::rand() / RAND_MAX;
    pArray->add(i, index);
  }
  std::cout << stopwatch.sinceStartMs() << std::endl;
}

template<typename T>
void RemoveFromHeadTest(IArray<T>* pArray, std::string const& sArrayType)
{
  size_t nTotalElements = 100000;

  std::cout << sArrayType << ": ";
  for(size_t i = 0; i < nTotalElements; ++i)
    pArray->add(i);

  Stopwatch stopwatch;
  stopwatch.start();

  for (size_t i = 0; i < nTotalElements; ++i)
    pArray->remove(0);
  std::cout << stopwatch.sinceStartMs() << std::endl;
}

template<typename T>
void RemoveFromTailTest(IArray<T>* pArray, std::string const& sArrayType)
{
  size_t nTotalElements = 100000;

  std::cout << sArrayType << ": ";
  for(size_t i = 0; i < nTotalElements; ++i)
    pArray->add(i);

  Stopwatch stopwatch;
  stopwatch.start();

  for (size_t i = 0; i < nTotalElements; ++i)
    pArray->remove(pArray->size() - 1);
  std::cout << stopwatch.sinceStartMs() << std::endl;
}

template<typename T>
void RandomRemoveTest(IArray<T>* pArray, std::string const& sArrayType)
{
  size_t nTotalElements = 100000;

  std::cout << sArrayType << ": ";
  for(size_t i = 0; i < nTotalElements; ++i)
    pArray->add(i);

  Stopwatch stopwatch;
  stopwatch.start();

  std::srand(200);
  for (size_t i = 0; i < nTotalElements; ++i)
  {
    size_t index = pArray->size() * std::rand() / RAND_MAX;
    pArray->remove(index);
  }
  std::cout << stopwatch.sinceStartMs() << std::endl;
}


int main(int, char*[])
{
  std::cout << "Functional tests:" << std::endl;
  {
    std::cout << "SingleArray: ";
    SingleArray<size_t> array;
    std::cout << (functionalTests(array) ? "Passed" : "FAILED") << std::endl;
  }
  {
    std::cout << "VectorArray: ";
    VectorArray<size_t> array(3);
    std::cout << (functionalTests(array) ? "Passed" : "FAILED") << std::endl;
  }
  {
    std::cout << "FactorArray: ";
    FactorArray<size_t> array(3, 2);
    std::cout << (functionalTests(array) ? "Passed" : "FAILED") << std::endl;
  }
  {
    std::cout << "MatrixArray: ";
    MatrixArray<size_t> array(4, new VectorArray<IArray<size_t>*>(10));
    std::cout << (functionalTests(array) ? "Passed" : "FAILED") << std::endl;
  }
  std::cout << std::endl;

  std::cout << "\n--------------------------------------------------------\n";
  std::cout << "Append elements (10000, 100000 and 1000000 elements):" << std::endl;
  //AppendTest<SingleArray<size_t>>        ("SingleArray      ");
  AppendTest<VectorArray<size_t>, size_t>("VectorArray(100) ",  100);
  AppendTest<VectorArray<size_t>, size_t>("VectorArray(1000)", 1000);
  AppendTest<FactorArray<size_t>, size_t, double>("FactorArray(10, 1.1)", 10, 1.1);
  AppendTest<FactorArray<size_t>, size_t, double>("FactorArray(10, 1.5)", 10, 1.5);
  AppendTest<FactorArray<size_t>, size_t, double>("FactorArray(10, 2)",   10, 2);
  AppendTest<MatrixArray<size_t>, size_t>("MatrixArray(10)  ",  10);
  AppendTest<MatrixArray<size_t>, size_t>("MatrixArray(100) ",  100);
  AppendTest<MatrixArray<size_t>, size_t>("MatrixArray(1000)",  1000);

  std::cout << "\n--------------------------------------------------------\n";
  std::cout << "Insert to head (100000 elements):" << std::endl;
  InsertToHeadTest(new SingleArray<size_t>(), "SingleArray");
  InsertToHeadTest(new VectorArray<size_t>(10),   "VectorArray(10)  ");
  InsertToHeadTest(new VectorArray<size_t>(100),  "VectorArray(100) ");
  InsertToHeadTest(new VectorArray<size_t>(1000), "VectorArray(1000)");
  InsertToHeadTest(new FactorArray<size_t>(10, 1.1), "FactorArray(10, 1.1)");
  InsertToHeadTest(new FactorArray<size_t>(10, 1.5),  "FactorArray(10, 1.5)");
  InsertToHeadTest(new FactorArray<size_t>(10, 2),    "FactorArray(10, 2)");
  InsertToHeadTest(new MatrixArray<size_t>(10),   "MatrixArray(10)  ");
  InsertToHeadTest(new MatrixArray<size_t>(100),  "MatrixArray(100) ");
  InsertToHeadTest(new MatrixArray<size_t>(1000), "MatrixArray(1000)");

  std::cout << "\n--------------------------------------------------------\n";
  std::cout << "Insert to tail (100000 elements):" << std::endl;
  RandomInsertTest(new SingleArray<size_t>(), "SingleArray");
  InsertToTailTest(new VectorArray<size_t>(10),   "VectorArray(10)  ");
  InsertToTailTest(new VectorArray<size_t>(100),  "VectorArray(100) ");
  InsertToTailTest(new VectorArray<size_t>(1000), "VectorArray(1000)");
  InsertToTailTest(new FactorArray<size_t>(10, 1.1), "FactorArray(10, 1.1)");
  InsertToTailTest(new FactorArray<size_t>(10, 1.5),  "FactorArray(10, 1.5)");
  InsertToTailTest(new FactorArray<size_t>(10, 2),    "FactorArray(10, 2)");
  InsertToTailTest(new MatrixArray<size_t>(10),   "MatrixArray(10)  ");
  InsertToTailTest(new MatrixArray<size_t>(100),  "MatrixArray(100) ");
  InsertToTailTest(new MatrixArray<size_t>(1000), "MatrixArray(1000)");

  std::cout << "\n--------------------------------------------------------\n";
  std::cout << "Random insert (100000 elements):" << std::endl;
  RandomInsertTest(new SingleArray<size_t>(), "SingleArray");
  RandomInsertTest(new VectorArray<size_t>(10),   "VectorArray(10)  ");
  RandomInsertTest(new VectorArray<size_t>(100),  "VectorArray(100) ");
  RandomInsertTest(new VectorArray<size_t>(1000), "VectorArray(1000)");
  RandomInsertTest(new FactorArray<size_t>(10, 1.1), "FactorArray(10, 1.1)");
  RandomInsertTest(new FactorArray<size_t>(10, 1.5),  "FactorArray(10, 1.5)");
  RandomInsertTest(new FactorArray<size_t>(10, 2),    "FactorArray(10, 2)");
  RandomInsertTest(new MatrixArray<size_t>(10),   "MatrixArray(10)  ");
  RandomInsertTest(new MatrixArray<size_t>(100),  "MatrixArray(100) ");
  RandomInsertTest(new MatrixArray<size_t>(1000), "MatrixArray(1000)");

  std::cout << "\n--------------------------------------------------------\n";
  std::cout << "Remove from head (100000 elements):" << std::endl;
  RemoveFromHeadTest(new SingleArray<size_t>(), "SingleArray");
  RemoveFromHeadTest(new VectorArray<size_t>(10),   "VectorArray(10)  ");
  RemoveFromHeadTest(new VectorArray<size_t>(100),  "VectorArray(100) ");
  RemoveFromHeadTest(new VectorArray<size_t>(1000), "VectorArray(1000)");
  RemoveFromHeadTest(new FactorArray<size_t>(10, 1.1), "FactorArray(10, 1.1)");
  RemoveFromHeadTest(new FactorArray<size_t>(10, 1.5),  "FactorArray(10, 1.5)");
  RemoveFromHeadTest(new FactorArray<size_t>(10, 2),    "FactorArray(10, 2)");
  RemoveFromHeadTest(new MatrixArray<size_t>(10),   "MatrixArray(10)  ");
  RemoveFromHeadTest(new MatrixArray<size_t>(100),  "MatrixArray(100) ");
  RemoveFromHeadTest(new MatrixArray<size_t>(1000), "MatrixArray(1000)");

  std::cout << "\n--------------------------------------------------------\n";
  std::cout << "Remove from tail (100000 elements):" << std::endl;
  RemoveFromTailTest(new SingleArray<size_t>(), "SingleArray");
  RemoveFromTailTest(new VectorArray<size_t>(10),   "VectorArray(10)  ");
  RemoveFromTailTest(new VectorArray<size_t>(100),  "VectorArray(100) ");
  RemoveFromTailTest(new VectorArray<size_t>(1000), "VectorArray(1000)");
  RemoveFromTailTest(new FactorArray<size_t>(10, 1.1), "FactorArray(10, 1.1)");
  RemoveFromTailTest(new FactorArray<size_t>(10, 1.5),  "FactorArray(10, 1.5)");
  RemoveFromTailTest(new FactorArray<size_t>(10, 2),    "FactorArray(10, 2)");
  RemoveFromTailTest(new MatrixArray<size_t>(10),   "MatrixArray(10)  ");
  RemoveFromTailTest(new MatrixArray<size_t>(100),  "MatrixArray(100) ");
  RemoveFromTailTest(new MatrixArray<size_t>(1000), "MatrixArray(1000)");

  std::cout << "\n--------------------------------------------------------\n";
  std::cout << "Random remove (100000 elements):" << std::endl;
  RandomRemoveTest(new SingleArray<size_t>(), "SingleArray");
  RandomRemoveTest(new VectorArray<size_t>(10),   "VectorArray(10)  ");
  RandomRemoveTest(new VectorArray<size_t>(100),  "VectorArray(100) ");
  RandomRemoveTest(new VectorArray<size_t>(1000), "VectorArray(1000)");
  RandomRemoveTest(new FactorArray<size_t>(10, 1.1), "FactorArray(10, 1.1)");
  RandomRemoveTest(new FactorArray<size_t>(10, 1.5),  "FactorArray(10, 1.5)");
  RandomRemoveTest(new FactorArray<size_t>(10, 2),    "FactorArray(10, 2)");
  RandomRemoveTest(new MatrixArray<size_t>(10),   "MatrixArray(10)  ");
  RandomRemoveTest(new MatrixArray<size_t>(100),  "MatrixArray(100) ");
  RandomRemoveTest(new MatrixArray<size_t>(1000), "MatrixArray(1000)");
  return 0;
}
