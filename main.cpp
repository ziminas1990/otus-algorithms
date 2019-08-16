#include "AwesomeHeap.h"
#include <vector>
#include <algorithm>  // for random_shuufle
#include <assert.h>
#include <iostream>
#include "Utils.h"

// Данная структура используется в автотестах. Хранит в себе информацию о
// некотором выделенном участке памяти, а именно длину выделенного участка
// и значения, которыми должен быть заполнен данный участок памяти
struct AllocatedChunk {
  uint16_t* pData           = nullptr;
  size_t    nLength         = 0;
  uint16_t  nExpectedValues = 0;
};

// Данная функция запрашивает в куче выделение памяти для nTotal массивов
// случайной длины (до 512 байт) и заполняет каждый массив некоторым
// случайным значением
void allocateRandomChunks(std::vector<AllocatedChunk>& chunks, size_t nTotal,
                          AwesomeHeap& heap)
{
  for (size_t i = 0; i < nTotal; ++i) {
    AllocatedChunk& chunk = chunks[i];
    chunk.nExpectedValues = rand() % 0xFFFF;
    chunk.nLength         = rand() % 512;
    chunk.pData           = (uint16_t*)(heap.allocate(chunk.nLength * sizeof(uint16_t)));
    assert(chunk.pData);
    for (size_t j = 0; j < chunk.nLength; ++j)
      chunk.pData[j] = chunk.nExpectedValues;
  }
}

// Данная функция освобождаем nTotal первых массивов из chunks
void freeRandomChunks(std::vector<AllocatedChunk>& chunks, size_t nTotal,
                      AwesomeHeap& heap)
{
  for (size_t j = 0; j < nTotal; ++j) {
    AllocatedChunk& chunk = chunks[j];
    heap.free(chunk.pData);
    chunk = AllocatedChunk();
  }
}

// Данная функция проверяет, что все выделенные массивы заполнены
// теми значениями, которыми ожидается
// Такая проверка гарантирует, что менеджер памяти не допустит
// ошибки, из-за которой клиенту будут переданы два участка памяти,
// перекрвающих друг друга полностью или частично
bool checkChunks(std::vector<AllocatedChunk> const& chunks)
{
  for (AllocatedChunk const& chunk : chunks)
    for (size_t j = 0; j < chunk.nLength; ++j)
      if (chunk.pData[j] != chunk.nExpectedValues)
        return false;
  return true;
}

bool Test(AwesomeHeap& heap)
{
  // Идея теста:
  // 1. выделяем N массивов (чанков) случаной длины и заполняем их
  //    случайными значениями (т.е. создаём чанки AllocatedChunk)
  // 2. перемешиваем созданные чанки
  // 3. освобождаем память, занятую под первые M чанков; M - случайное
  //    число от 0 до N/2;
  // 4. вместо освобождённых M чанков создаём новые M чанков, так же
  //    со случайно длиной и заполнением;
  // 5. проверяем, что все N созданных чанков валидны (содержимое
  //    ни одного из чанков не было повреждено)
  // 6. пункт 2 - 5 выполяем 1000 раз, причём на каждой 100й итерации
  //    выводим степень фрагментации памяти
  // 7. если в процессе выполнения пунктов 2-6 не было обнаружено ни
  //    одного повреждённого чанка, то считаем что тест пройден успешно
  const size_t nTotalChunks = 1000;

  // п. 1:
  std::vector<AllocatedChunk> chunks;
  chunks.resize(nTotalChunks);
  allocateRandomChunks(chunks, nTotalChunks, heap);

  for(size_t i = 0; i < 1000; ++i) {
    // п. 2:
    std::random_shuffle(chunks.begin(), chunks.end());

    size_t nElementsToReallocate = std::rand() % (nTotalChunks / 2);
    // п. 3:
    freeRandomChunks(chunks, nElementsToReallocate, heap);
    // п. 4:
    allocateRandomChunks(chunks, nElementsToReallocate, heap);
    // п. 5:
    if (!checkChunks(chunks))
      return false;
    if (i % 100 == 0) {
      std::cout << "Iteration #" << i << ": " << heap.getFragmentationLevel() << std::endl;
    }
  }
  return true;
}

std::ostream& operator<<(std::ostream& out, AwesomeHeap::AllocatePolicy ePolicy)
{
  switch(ePolicy) {
    case AwesomeHeap::eBest: return out << "Best";
    case AwesomeHeap::eFirst: return out << "First";
    case AwesomeHeap::eWorst: return out << "Worst";
  }
}

int main(int argc, char* argv[])
{
  std::srand(time(nullptr));

  // Прогоняем тест длы AswesomeHeap для всех доступных политик, замеряем время теста
  // и степень фрагментации памяти в процессе выполнения теста.
  for(AwesomeHeap::AllocatePolicy policy : AwesomeHeap::AllPolicies())
  {
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "Testing AwesomeHeap with \"" << policy <<  "\" policy..." << std::endl;
    AwesomeHeap heap(1000 * 1024, policy);
    Stopwatch stopwatch;
    stopwatch.start();
    bool lSuccess = Test(heap);
    long duration = stopwatch.sinceStartMs();
    std::cout << "Test status: " << (lSuccess ? "OK" : "FAILED!") << " (in " <<
                 duration << " ms)" << std::endl;
  }
  return 0;
}
