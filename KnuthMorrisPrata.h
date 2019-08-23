#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include <assert.h>

class Alphabet : public std::vector<uint8_t>
{
public:
  inline uint8_t getCode(uint8_t nSymbol) const {
    assert(nSymbol >= at(0));
    uint8_t code = nSymbol - at(0);
    assert(code < size());
    return code;
  }
};

// index - alphabet's symbol code, value - new state
using Switches      = size_t*;
// index - position in pattern, value - switches
using SwitchesTable = Switches*;

inline bool startsWith(char const* sText, char const* sPrefix, size_t nPrefixLength)
{
  for(size_t i = 0; i < nPrefixLength; ++i)
    if (sText[i] != sPrefix[i])
      return false;
  return true;
}

size_t getLongestCommonSuffix(char const* sString, size_t nLength)
{
  size_t nLongestSuffix = 0;
  for(size_t nSuffix = 1; nSuffix < nLength; ++nSuffix)
    if (startsWith(sString, sString + nLength - nSuffix, nSuffix))
      nLongestSuffix = nSuffix;
  return nLongestSuffix;
}

Switches precalculateSwitches(char const* sString, size_t nLength,
                              Alphabet const& alphabet)
{
  char nStoredLastSym = sString[nLength - 1];

  Switches switches = new size_t[alphabet.size()];

  char* sMutableString = const_cast<char*>(sString);
  for (uint8_t sym : alphabet) {
    sMutableString[nLength - 1] = char(sym);
    switches[alphabet.getCode(sym)] = getLongestCommonSuffix(sMutableString, nLength);
  }
  switches[alphabet.getCode(nStoredLastSym)] = nLength;
  sMutableString[nLength - 1] = nStoredLastSym;
  return switches;
}


SwitchesTable buildSwitchesTable(char const* sPattern, size_t nLength,
                                 Alphabet const& alphabet)
{
  SwitchesTable table;
  table = new Switches[nLength];

  for (size_t i = 1; i <= nLength; ++i)
    table[i - 1] = precalculateSwitches(sPattern, i, alphabet);

  uint8_t nLastSymbolCode = alphabet.getCode(sPattern[nLength - 1]);
  table[nLength - 1][nLastSymbolCode] = getLongestCommonSuffix(sPattern, nLength);

  return table;
}

std::vector<size_t> KnuthMorrisPrataSearch(
    std::string const& sPattern, std::string const& sText,
    SwitchesTable const& switchTable,  Alphabet const& alphabet)
{
  std::vector<size_t> matches;
  size_t nState = 0;
  for(size_t i = 0; i < sText.size(); ++i) {
    if (sPattern[nState] == sText[i])
      if (nState == sPattern.size() - 1)
        matches.push_back(i - nState);
    nState = switchTable[nState][alphabet.getCode(sText[i])];
  }
  return matches;
}

std::vector<size_t> KnuthMorrisPrataSearch(std::string const& sPattern,
                                           std::string const& sText,
                                           Alphabet const& alphabet)
{
  SwitchesTable table  = buildSwitchesTable(sPattern.c_str(), sPattern.size(), alphabet);
  return KnuthMorrisPrataSearch(sPattern, sText, table, alphabet);
}
