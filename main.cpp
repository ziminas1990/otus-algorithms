#include <vector>
#include <algorithm>  // for random_shuffle
#include <assert.h>
#include <iostream>

#include "BoyerMooreHorspool.h"
#include "KnuthMorrisPrata.h"
#include "Utils.h"

std::vector<size_t> NaiveSearch(std::string const& sPattern, std::string const& sText)
{
  std::vector<size_t> matches;

  for(size_t i = 0; i < sText.size() - sPattern.size() + 1; ++i) {
    for (size_t j = 0; j < sPattern.size(); ++j) {
      if (sText[i + j] != sPattern[j])
        break;
      if (j == sPattern.size() - 1)
        matches.push_back(i);
    }
  }
  return matches;
}

std::string generateText(Alphabet const& alphabet, size_t nTextLength)
{
  std::string sText(nTextLength, alphabet[0]);
  for(size_t i = 0; i < nTextLength; ++i)
    sText[i] = alphabet[std::rand() % alphabet.size()];
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
  Alphabet alphabet;
  for (char sym : "ABCDEFG")
    alphabet.push_back(sym);
  alphabet.pop_back();

  std::string sText     = generateText(alphabet, 100000);
  std::string sPattern  = getRandomSubstring(sText, 3, 12);
  return BoyerMooreHorspoolSearch(sPattern, sText) == NaiveSearch(sPattern, sText);
}

bool TestKnuthMorrisPrataSearch()
{
  Alphabet alphabet;
  for (char sym : "ABCDEFG")
    alphabet.push_back(sym);
  alphabet.pop_back();

  std::string sText     = generateText(alphabet, 100000);
  std::string sPattern  = getRandomSubstring(sText, 3, 12);
  return KnuthMorrisPrataSearch(sPattern, sText, alphabet) == NaiveSearch(sPattern, sText);
}

int main(int argc, char* argv[])
{
  auto nSeed = time(nullptr);
  {
    Stopwatch stopwatch;
    stopwatch.start();
    auto sStatus = RunRandomizedTest(TestBoyerMooreHorpoolSearch, nSeed, 500);
    auto nDuration = stopwatch.sinceStartMs();
    std::cout << "Testing Boyer-Moore-Horpool: " << sStatus <<
                 " (" << nDuration << " ms)" << std::endl;
  }
  {
    Stopwatch stopwatch;
    stopwatch.start();
    auto sStatus = RunRandomizedTest(TestKnuthMorrisPrataSearch, nSeed, 500);
    auto nDuration = stopwatch.sinceStartMs();
    std::cout << "Testing Knuth-Morris-Prata: " << sStatus <<
                 " (" << nDuration << " ms)" << std::endl;
  }
  return 0;
}
