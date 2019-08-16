#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>

class AwesomeHeap
{
#pragma pack (push, 1)
  // Заголовок чанка. Каждый чанк хранит в себе указатели на заголовки
  // предыдущего и последующего чанка в куче (соседние чанки), а так же
  // признак занятости чанка
  // После заголовка вполть до следующего чанка расположзена память,
  // которая может быть передана под нужды клиентского кода
  struct Chunk {
    uint8_t* asRawPointer() const {
      return const_cast<uint8_t*>(reinterpret_cast<uint8_t const*>(this));
    }
    // returns length of pUserData array
    size_t length() const { return pNext->asRawPointer() - pUserData; }

    Chunk*   pNext     = nullptr;
    Chunk*   pPrevious = nullptr;
    bool     lFree     = true;
    uint8_t  pUserData[]; // see "flexable array member"
  };
#pragma pack(pop)

public:
  // Политики поиска подходящего чанка:
  enum AllocatePolicy {
    eFirst, // первый, достаточно большой
    eBest,  // самый маленький из всех достаточно больших чанков
    eWorst  // самый большой чанк
  };

  static std::vector<AllocatePolicy> AllPolicies() { return {eFirst, eBest, eWorst}; }

  AwesomeHeap(size_t nHeapSize, AllocatePolicy ePolicy);
  ~AwesomeHeap() { delete[] m_pArena; }

  void* allocate(size_t nSize);
  void  free(void* pData);

  // В качестве степени фрагментации будем считать отношение количества свободных
  // блоков к количеству занятых. В идеале, количество свободных блоков - 1, а
  // количество занятых - много больше, поэтому степень фрагментации стремится к 0
  // Если свободных блоков много, то уровень фрагментации увеличивается
  double getFragmentationLevel() const {
    return double(m_Stat.nTotalFreeBlocks) / m_Stat.nTotalOccupiedBlocks;
  }

private:
  Chunk* asChunk(void *pData) {
    return reinterpret_cast<Chunk*>(reinterpret_cast<uint8_t*>(pData) - sizeof(Chunk));
  }

  Chunk* findFirstChunk(size_t nSize);
  Chunk* findBestChunk(size_t nSize);
  Chunk* findWorstChunk(size_t nSize);

  Chunk* findChunk(size_t nSize) {
    switch (m_ePolicy) {
      case eFirst: return findFirstChunk(nSize);
      case eBest:  return findBestChunk(nSize);
      case eWorst: return findWorstChunk(nSize);
    }
  }

  // Присоединяет чанк pRight к чанку pLeft, после чего чанк pRight
  // перестаёт существовать
  inline void leftJoin(Chunk* pLeft, Chunk* pRight)
  {
    pLeft->pNext = pRight->pNext;
    pLeft->pNext->pPrevious = pLeft;
    --m_Stat.nTotalFreeBlocks;
  }

  // Данные функции используются для целей отладки:
  inline bool checkPointer(void* pPointer)
  {
    return !pPointer || (pPointer <= m_pLastChunk && pPointer >= m_pFirstChunk);
  }
  void checkChunk(Chunk* pChunk);
  void checkHeap();

private:
  AllocatePolicy m_ePolicy;

  uint8_t* m_pArena      = nullptr;
  Chunk*   m_pFirstChunk = nullptr;
  Chunk*   m_pLastChunk  = nullptr;

  // Статистика пишется исключительно в исследовательских целях
  // (чтобы можно было оценить уровень фрагментации памяти)
  struct Statistic {
    size_t nTotalOccupiedBlocks;
    size_t nTotalFreeBlocks;
  } m_Stat;
};
