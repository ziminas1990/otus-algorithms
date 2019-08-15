#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <vector>

class AwesomeHeap
{
#pragma pack (push, 1)
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
  enum AllocatePolicy {
    eFirst,
    eBest,
    eWorst
  };

  static std::vector<AllocatePolicy> AllPolicies() { return {eFirst, eBest, eWorst}; }

  AwesomeHeap(size_t nHeapSize, AllocatePolicy ePolicy);
  ~AwesomeHeap() { delete[] m_pArena; }

  void* allocate(size_t nSize);
  void  free(void* pData);

  // for research reasons:
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

  inline void leftJoin(Chunk* pLeft, Chunk* pRight)
  {
    pLeft->pNext = pRight->pNext;
    pLeft->pNext->pPrevious = pLeft;
    --m_Stat.nTotalFreeBlocks;
  }

  // for debugginh purposes
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

  struct Statistic {
    size_t nTotalOccupiedBlocks;
    size_t nTotalFreeBlocks;
  } m_Stat;
};
