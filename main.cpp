#include <vector>
#include <algorithm>  // for random_shuffle
#include <assert.h>
#include <iostream>

#include "BoyerMooreHorspool.h"
#include "KnuthMorrisPrata.h"
#include "Utils.h"

// Наивная реализация поиска подстроки в строке. Результат её работы будет использоваться
// как эталон для проверки других алгоритмах в автотестах
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

// Генерирует произвольный текст длиной nTextLength и алфавитом alphabet
std::string generateText(Alphabet const& alphabet, size_t nTextLength)
{
  std::string sText(nTextLength, alphabet[0]);
  for(size_t i = 0; i < nTextLength; ++i)
    sText[i] = alphabet[std::rand() % alphabet.size()];
  return sText;
}

// Выбирает из текста произвольную подстроку длиной от nMinLength до nMaxLength
std::string getRandomSubstring(std::string const& sText, size_t nMinLength,
                                size_t nMaxLength)
{
  size_t nLength = nMinLength + std::rand() % (nMaxLength - nMinLength + 1);
  size_t nOffset = std::rand() % (sText.size() - nLength);
  return sText.substr(nOffset, nLength);
}

// Функция тестирует работу алгоритма БМХ
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

// Функция тестирует работу алгоритма КМП
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

// Функция производит стократный прогон каждого из алгоритмов на одном и том же наборе
// данных и для каждого алгоритм измеряет время прогона. Время выводится сразу в консоль.
bool PerfomanceTest(size_t nTextLength)
{
  Alphabet alphabet;
  for (char sym : "ABCDEFGHIJKLMNOP")
    alphabet.push_back(sym);
  alphabet.pop_back();

  std::string sText    = generateText(alphabet, nTextLength);
  std::string sPattern = getRandomSubstring(sText, 15, 15);

  Stopwatch stopwatch;

  stopwatch.start();
  for (size_t i = 0; i < 100; ++i)
    NaiveSearch(sPattern, sText);
  std::cout << stopwatch.sinceStartMs() << " ";

  stopwatch.start();
  for (size_t i = 0; i < 100; ++i)
    BoyerMooreHorspoolSearch(sPattern, sText);
  std::cout << stopwatch.sinceStartMs() << " ";

  stopwatch.start();
  SwitchesTable switchTable = buildSwitchesTable(sPattern.c_str(), sPattern.size(), alphabet);
  for (size_t i = 0; i < 100; ++i)
    KnuthMorrisPrataSearch(sPattern, sText, switchTable, alphabet);
  std::cout << stopwatch.sinceStartMs() << std::endl;

  return true;
}

int main(int argc, char* argv[])
{
  if (argc < 2)
    return 0;

  std::string sMode = std::string(argv[1]);

  if (sMode == "autotests") {
    // Режим автотестов
    // Для каждого из алгоритмов запускает автотесты 500 раз (каждый раз
    // будут генерится новые входные данные) и возвращает результат проверки
    auto nSeed = time(nullptr);
    {
      Stopwatch stopwatch;
      stopwatch.start();
      auto sStatus = RunRandomizedTest(TestBoyerMooreHorpoolSearch, nSeed, 500);
      auto nDuration = stopwatch.sinceStartMs();
      std::cout << "Testing Boyer-Moore-Horspool: " << sStatus <<
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

  if (sMode == "perfomance") {
    // Режим сравнения производительности
    if (argc != 5)
      return 1;
    int nSmallText = atoi(argv[2]);
    int nBigText   = atoi(argv[3]);
    int nStep      = atoi(argv[4]);
    for (int nLength = nSmallText; nLength <= nBigText; nLength += nStep) {
      std::cout << nLength << " ";
      PerfomanceTest(nLength);
    }

    return 0;
  }

  if (sMode == "trace") {
    // Режим трассировки алгоритма БМХ
    if (argc != 4)
      return 1;
    std::string sPattern = argv[2];
    std::string sText    = argv[3];
    auto matches = BoyerMooreHorspoolSearch(sPattern, sText, true);

    std::cout << matches.size() << " matches: ";
    for (size_t index : matches)
      std::cout << index << " ";
    std::cout << std::endl;
    return 0;
  }
  return 0;
}
