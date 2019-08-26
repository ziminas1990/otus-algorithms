#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include <assert.h>

// Данный класс позволяет преобразовать символ алфавита в его код
// Самим алфавитом выступает массив (точнее vector, от которого унаследован класс)
// Симолвы в алфавите должны иметь монотонно возрастающий ASCII код.
// Например "ABCDEF". Последовательность "ADGEI1234" не годится  в качестве
// алфавита, т.к. ASCII-коды её символов растут немонотонно.
// Такое ограничение введено для простоты и лучшей производительности
// Возвращаемые коды нумеруются с 0.
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

// Подробнее о типe SwitchesTable см в README.md (п. "Комментарии к коду").
// Индекс массива - это код некоторого символа в алфавите, значение ячейки в
// массиве - номер состояния, на которое нужно переключиться
using Switches      = size_t*;
// Индекс массива - это позиция в паттерне или номер состояния (что одно и
// то же), значение - это соответствие нового состояния каждому из символов алфавита 
using SwitchesTable = Switches*;

// Проверяет, что некоторый текст sText начинается с префикса sPrefix длиной nPrefixLength
// Вызывающий код должен гарантировать что длина текста sText больше длины префикса sPrefix
inline bool startsWith(char const* sText, char const* sPrefix, size_t nPrefixLength)
{
  for(size_t i = 0; i < nPrefixLength; ++i)
    if (sText[i] != sPrefix[i])
      return false;
  return true;
}

// Возвращает длину наиболее длинного суффикса строки, который одновременно является
// ей префиксом, либо 0, если такого суффикса нет
size_t getLongestCommonSuffix(char const* sString, size_t nLength)
{
  size_t nLongestSuffix = 0;
  for(size_t nSuffix = 1; nSuffix < nLength; ++nSuffix)
    if (startsWith(sString, sString + nLength - nSuffix, nSuffix))
      nLongestSuffix = nSuffix;
  return nLongestSuffix;
}

// Вспомогательная функция для buildSwitchesTable
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

// Функция, которая по указанному паттерну строит таблицу переключений
// Таблица описана в README.md (см. п. "Комментарии к коду").
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

// Реализация алгоритма КМП
std::vector<size_t> KnuthMorrisPrataSearch(
    std::string const& sPattern, std::string const& sText,
    SwitchesTable const& switchTable,  Alphabet const& alphabet)
{
  std::vector<size_t> matches;
  size_t nState = 0; // начальное состояние - 0.
  // Двигаемся по всему тексту
  for(size_t i = 0; i < sText.size(); ++i) {
    if (sPattern[nState] == sText[i])
      if (nState == sPattern.size() - 1)
        matches.push_back(i - nState);
    // Переключаемся в новое состояние
    nState = switchTable[nState][alphabet.getCode(sText[i])];
  }
  return matches;
}

// Обёртка, которая берёт на себя задачу вычисления таблицы переходов
std::vector<size_t> KnuthMorrisPrataSearch(std::string const& sPattern,
                                           std::string const& sText,
                                           Alphabet const& alphabet)
{
  SwitchesTable table  = buildSwitchesTable(sPattern.c_str(), sPattern.size(), alphabet);
  return KnuthMorrisPrataSearch(sPattern, sText, table, alphabet);
}
