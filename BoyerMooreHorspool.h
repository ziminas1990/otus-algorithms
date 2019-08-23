#pragma once

#include <vector>
#include <iostream>

using Offsets      = std::vector<size_t>;
using OffestsTable = std::vector<Offsets>;

static OffestsTable compile(std::string const& sPattern)
{
  OffestsTable table;
  table.resize(256);
  for (uint8_t sPatternSymbol : sPattern) {
    Offsets& offsets = table[sPatternSymbol];
    if (!offsets.empty())
      continue;
    offsets.reserve(sPattern.size());
    size_t nOffset = 1;
    for (uint8_t sTextSymbol : sPattern) {
      if (sTextSymbol == sPatternSymbol)
        nOffset = 0;
      offsets.push_back(nOffset);
      ++nOffset;
    }
  }
  return table;
}

static  void printState(std::string const& sPattern, std::string const& sText,
                        size_t nOffset)
{
  std::cout << "Offset: " << nOffset << std::endl;
  std::cout << sText << std::endl;
  for (size_t i = 0; i < nOffset + sPattern.size(); ++i)
    std::cout << ((i < nOffset) ? ' ' : sPattern[i - nOffset]);
  std::cout << std::endl;
}

inline size_t applyPattern(std::string const& sPattern, std::string const& sText,
                           size_t nTextOffset, OffestsTable const& table)
{
  size_t nPatternOffset = sPattern.size() - 1;
  size_t nOffset        = nTextOffset + nPatternOffset;
  while (nOffset > nTextOffset) {
    if (sText[nOffset] == sPattern[nPatternOffset]) {
      --nPatternOffset;
      --nOffset;
    } else {
      Offsets const& offsets = table[sText[nOffset]];
      if (offsets.empty()) {
        return (offsets.empty()) ? nOffset - nTextOffset : offsets[nPatternOffset];
      } else {
        return offsets[nPatternOffset];
      }
    }
  }

  return sText[nOffset] == sPattern[0] ? 0 : 1;
}

static std::vector<size_t> BoyerMooreHorspoolSearch(
    std::string const& sPattern, std::string const& sText, bool lPrintTrace = false)
{
  std::vector<size_t> matches;

  if (sText.size() < sPattern.size())
    return matches;

  OffestsTable table      = compile(sPattern);
  size_t nTextOffset      = 0;
  size_t nTextOffsetLimit = sText.size() - sPattern.size();
  while(nTextOffset <= nTextOffsetLimit) {
    if (lPrintTrace)
      printState(sPattern, sText, nTextOffset);
    size_t nOffset = applyPattern(sPattern, sText, nTextOffset, table);
    if (!nOffset) {
      matches.push_back(nTextOffset++);
    } else {
      nTextOffset += nOffset;
    }
  }
  return matches;
}
