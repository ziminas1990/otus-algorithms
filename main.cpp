#include <vector>
#include <algorithm>  // for random_shuffle
#include <assert.h>
#include <iostream>
#include "Utils.h"


using Offsets      = std::vector<size_t>;
using OffestsTable = std::vector<Offsets>;

OffestsTable compile(std::string const& sPattern)
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

void printState(std::string const& sPattern, std::string const& sText, size_t nOffset)
{
  std::cout << "Offset: " << nOffset << std::endl;
  std::cout << sText << std::endl;
  for (size_t i = 0; i < sText.size(); ++i)
    if (i < nOffset || i > nOffset + sPattern.size())
      std::cout << ' ';
    else
      std::cout << sPattern[i - nOffset];
  std::cout << std::endl << std::endl;
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

std::vector<size_t> BoyerMooreHorspoolSearch(std::string const& sPattern,
                                            std::string const& sText,
                                            bool lPrintTrace = false)
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

std::vector<size_t> NaiveSearch(std::string const& sPattern, std::string const& sText)
{
  std::vector<size_t> matches;

  for(size_t i = 0; i < sText.size() - sPattern.size(); ++i) {
    for (size_t j = 0; j < sPattern.size(); ++j) {
      if (sText[i + j] != sPattern[j])
        break;
      if (j == sPattern.size() - 1)
        matches.push_back(i);
    }
  }
  return matches;
}

std::string generateText(std::string const& sAlphabet, size_t nTextLength)
{
  std::string sText(nTextLength, sAlphabet[0]);
  for(size_t i = 0; i < nTextLength; ++i)
    sText[i] = sAlphabet[std::rand() % sAlphabet.size()];
  return sText;
}

std::string getRandomSubstring(std::string const& sText, size_t nMinLength,
                                size_t nMaxLength)
{
  size_t nLength = nMinLength + std::rand() % (nMaxLength - nMinLength + 1);
  size_t nOffset = std::rand() % (sText.size() - nLength);
  return sText.substr(nOffset, nLength);
}

bool TestBoyerMooreHorpoolSearch()
{
  std::string sAlphabet = "ABCDEFG ";
  std::string sText     = generateText(sAlphabet, 10000);
  std::string sPattern  = getRandomSubstring(sText, 3, 10);
  return BoyerMooreHorspoolSearch(sPattern, sText) == NaiveSearch(sPattern, sText);
}

int main(int argc, char* argv[])
{
  std::cout << "Testing: " <<
               RunRandomizedTest(TestBoyerMooreHorpoolSearch, time(nullptr), 100000) <<
               std::endl;
  return 0;
}
