#include "AwesomeHeap.h"

#include <algorithm>  // for std::max only
#include <assert.h>

inline size_t align(size_t nSize)
{
  size_t nRemainder = nSize % 8;
  return nRemainder ? nSize + (8 - nRemainder) : nSize;
}

inline size_t distance(void* pLeft, void* pRight) {
  return reinterpret_cast<uint8_t*>(pRight) - reinterpret_cast<uint8_t*>(pLeft);
}

AwesomeHeap::AwesomeHeap(size_t nHeapSize, AllocatePolicy ePolicy)
  : m_ePolicy(ePolicy), m_pArena(new uint8_t[nHeapSize]),
    m_pFirstChunk(reinterpret_cast<Chunk*>(m_pArena)),
    m_pLastChunk(asChunk(m_pArena + nHeapSize))
{
  m_pFirstChunk->lFree     = true;
  m_pFirstChunk->pNext     = m_pLastChunk;
  m_pFirstChunk->pPrevious = nullptr;

  m_pLastChunk->lFree      = false;
  m_pLastChunk->pNext      = nullptr;
  m_pLastChunk->pPrevious  = m_pFirstChunk;

  m_Stat.nTotalFreeBlocks     = 1;
  m_Stat.nTotalOccupiedBlocks = 0;
}

void* AwesomeHeap::allocate(size_t nSize)
{
  size_t nAlignedSize = align(nSize + sizeof(Chunk)) - sizeof(Chunk);
  assert((sizeof(Chunk) + nAlignedSize) % 8 == 0);

  Chunk* pOccupiedChunk = findChunk(nAlignedSize);
  if (!pOccupiedChunk)
    return nullptr;
  pOccupiedChunk->lFree = false;

  ++m_Stat.nTotalOccupiedBlocks;

  Chunk* pTailChunk = reinterpret_cast<Chunk*>(pOccupiedChunk->pUserData + nAlignedSize);
  assert(pTailChunk <= pOccupiedChunk->pNext);

  if (distance(pTailChunk, pOccupiedChunk->pNext) > (3 * sizeof(Chunk))) {
    pTailChunk->lFree     = true;
    pTailChunk->pPrevious = pOccupiedChunk;
    pTailChunk->pNext     = pOccupiedChunk->pNext;
    pTailChunk->pNext->pPrevious = pTailChunk;
    pOccupiedChunk->pNext = pTailChunk;
  } else {
    assert(m_Stat.nTotalFreeBlocks > 0);
    --m_Stat.nTotalFreeBlocks;
  }
  return pOccupiedChunk->pUserData;
}

void AwesomeHeap::free(void *pData)
{
  Chunk* pReleasedChunk = asChunk(pData);

  pReleasedChunk->lFree = true;

  ++m_Stat.nTotalFreeBlocks;
  assert(m_Stat.nTotalOccupiedBlocks > 0);
  --m_Stat.nTotalOccupiedBlocks;

  Chunk* pNextChunk = pReleasedChunk->pNext;
  assert(pNextChunk != nullptr);
  if (pNextChunk->lFree && pNextChunk != m_pLastChunk)
    leftJoin(pReleasedChunk, pNextChunk);

  Chunk* pPreviousChunk = pReleasedChunk->pPrevious;
  if (pPreviousChunk && pPreviousChunk->lFree)
    leftJoin(pPreviousChunk, pReleasedChunk);
}

AwesomeHeap::Chunk* AwesomeHeap::findFirstChunk(size_t nSize)
{
  Chunk* pCurrentChunk = m_pFirstChunk;
  do {
    if (pCurrentChunk->lFree && pCurrentChunk->length() >= nSize)
      return pCurrentChunk;
    pCurrentChunk = pCurrentChunk->pNext;
  } while (pCurrentChunk != m_pLastChunk);
  return nullptr;
}

AwesomeHeap::Chunk* AwesomeHeap::findBestChunk(size_t nSize)
{
  Chunk* pBestChunk    = nullptr;
  Chunk* pCurrentChunk = m_pFirstChunk;

  do {
    if (pCurrentChunk->lFree && pCurrentChunk->length() >= nSize) {
      if (!pBestChunk || pCurrentChunk->length() < pBestChunk->length())
        pBestChunk = pCurrentChunk;
    }
    pCurrentChunk = pCurrentChunk->pNext;
  } while (pCurrentChunk != m_pLastChunk);

  return pBestChunk;
}

AwesomeHeap::Chunk* AwesomeHeap::findWorstChunk(size_t nSize)
{
  Chunk* pBestChunk    = nullptr;
  Chunk* pCurrentChunk = m_pFirstChunk;

  do {
    if (pCurrentChunk->lFree && pCurrentChunk->length() >= nSize) {
      if (!pBestChunk || pCurrentChunk->length() > pBestChunk->length())
        pBestChunk = pCurrentChunk;
    }
    pCurrentChunk = pCurrentChunk->pNext;
  } while (pCurrentChunk != m_pLastChunk);

  return pBestChunk;
}

void AwesomeHeap::checkChunk(AwesomeHeap::Chunk *pChunk)
{
  if (!pChunk)
    return;
  assert(checkPointer(pChunk));
  assert(checkPointer(pChunk->pNext));
  assert(checkPointer(pChunk->pPrevious));

  if (pChunk->pNext) {
    assert(checkPointer(pChunk->pNext->pNext));
    assert(pChunk->pNext->pPrevious == pChunk);
  }

  if (pChunk->pPrevious) {
    assert(checkPointer(pChunk->pPrevious->pPrevious));
    assert(pChunk->pPrevious->pNext == pChunk);
  }
}

void AwesomeHeap::checkHeap()
{
  Chunk* pCurrentChunk = m_pFirstChunk;
  while(pCurrentChunk != m_pLastChunk) {
    checkChunk(pCurrentChunk);
    pCurrentChunk = pCurrentChunk->pNext;
  }
}
