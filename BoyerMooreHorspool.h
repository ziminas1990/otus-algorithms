#pragma once

#include <vector>
#include <iostream>

// 
using Offsets      = std::vector<size_t>;
using OffestsTable = std::vector<Offsets>;

// Данная функция по переданному паттерну sPattern составляет
// таблицу смещений
// Подробнее см. README.md
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

// Используется исключительно для вывода лого в режиме трассировки
// Выводит на экран текущее значение смещения, под ним выводит текст sText
// (должен быть в одну строку!), а строкой ниже выводит паттерн sPattern
// со смещением на nOffset символов от начала строки
static void printState(std::string const& sPattern, std::string const& sText,
                       size_t nOffset)
{
  std::cout << "Offset: " << nOffset << std::endl;
  std::cout << sText << std::endl;
  for (size_t i = 0; i < nOffset + sPattern.size(); ++i)
    std::cout << ((i < nOffset) ? ' ' : sPattern[i - nOffset]);
  std::cout << std::endl;
}

// Алгоритм БМХ был разбит на две процедуры для лучшей читаемости. Данная процедура
// является внутренней для алгоритма.
// Если все символы паттерна, смещённого относительно текста sText на nTextOffset,
// совпадают с текстом, то функция возвращает 0. В противном случае возвращает
// количество символов, на которое нужно сместить паттерн относительно текста
// для того, чтобы продолжить поиск.
inline size_t applyPattern(std::string const& sPattern, std::string const& sText,
                           size_t nTextOffset, OffestsTable const& table)
{
  size_t nPatternOffset = sPattern.size() - 1;
  // nOffset - смещение относительно начала паттерна. Сравнивать паттерн
  // с текстом начинаем С КОНЦА паттерна.
  size_t nOffset        = nTextOffset + nPatternOffset;
  while (nOffset > nTextOffset) {
    if (sText[nOffset] == sPattern[nPatternOffset]) {
      // При совпадении символа просто двигаемся влево
      --nPatternOffset;
      --nOffset;
    } else {
      // При несовпадении символа смотрим, насколько нужно сместить паттерн,
      // чтобы под текущий символов текста sText[nOffset] оказался ближайший слева
      // аналогичный символ паттерна
      Offsets const& offsets = table[sText[nOffset]];
      if (offsets.empty()) {
        // Символ текста в паттерне вообще не встречается, значит можно сместить паттерн
        // к следующему символу текста
        return (offsets.empty()) ? nOffset - nTextOffset : offsets[nPatternOffset];
      } else {
        return offsets[nPatternOffset];
      }
    }
  }

  // Если мы здесь, значит все символы паттерна кроме первого гарантированно
  // совпали. Если совпал и первый, то это совпадение и возвращаем 0. Если первый
  // не совпал, то нужно сдвинуть паттерн на 1.
  return sText[nOffset] == sPattern[0] ? 0 : 1;
}

// Реализует алгоритм Бойера-Мура-Хорспула
// Возвращает массив вхождений паттерна sPattern в текст sText
// Если передать в качестве lPrintTrace значение true, то после каждого
// сдвига паттерна будет выводить в текст его текущую позицию
static std::vector<size_t> BoyerMooreHorspoolSearch(
    std::string const& sPattern, std::string const& sText, bool lPrintTrace = false)
{
  std::vector<size_t> matches;

  if (sText.size() < sPattern.size())
    return matches;

  // Вычисляем таблицу смещений:
  OffestsTable table      = compile(sPattern);
  size_t nTextOffset      = 0;  // Смещение паттерна относительно начала текста
  size_t nTextOffsetLimit = sText.size() - sPattern.size();
  while(nTextOffset <= nTextOffsetLimit) {
    if (lPrintTrace)
      printState(sPattern, sText, nTextOffset);
    // Сравниваем паттерн с текстом в текущей позиции
    size_t nOffset = applyPattern(sPattern, sText, nTextOffset, table);
    if (!nOffset) {
      // Есть совпадение! Значит сохраним его и сдвинем паттерн на 1
      matches.push_back(nTextOffset++);
    } else {
      // Паттерн не совпал с текстом в текущей позиции. Сдвигаем паттерн на
      // "рекомендованное" значение
      nTextOffset += nOffset;
    }
  }
  return matches;
}
