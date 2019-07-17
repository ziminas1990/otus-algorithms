
# ДЗ #5: Сортировка Шелла

Приложение принимает единственный аргумент - размер массива. Формирует случайный массив и запускает для него сортировку следующими алгоритмами:
  * сортировка вставкой;
  * сортировка Шелла с коэффициентами Шелла; ожидаемая сложность - O(N^2);
  * сортировка Шелла с коэффициентами Хиббарда; ожидаемая сложность - O(N^(3/2));
  * сортировка Шелла с коэффициентами Седвига; ожидаемая сложность - O(N^(4/3)).

Для **частично** отсортированных массивов эксперименты не проводились.

## Таблица производительности
Сортировка вставками:

| Алгоритм                     | 1000 | 2000 | 4000 | 6000 | 8000 | 10000 | 15000 | 20000 | 40000 | 70000 |
|------------------------------|------|------|------|------|------|-------|-------|-------|-------|-------|
| Сортировка вставками         | 2    | 12   | 47   | 96   | 171  | 267   | 600   | 1076  | 4324  | 13149 |

Сортировка Шелла:

| Алгоритм                     |  35к | 70к | 140к | 280к | 560к | 800к | 1000к | 1200к | 1400к  | 1600к | 2000к |
|------------------------------|------|-----|------|------|------|------|-------|-------|--------|-------|-------|
| Шелл (коэффициенты Шелла)    |  35  | 80  | 172  | 378  | 950  | 1616 | 1999  | 2463  | 2858   | 3935  | 4528  |
| Шелл (коэффициенты Хиббарда) |  31  | 73  | 163  | 381  | 899  | 1325 | 1772  | 2322  | 2779   | 3380  | 4243  |
| Шелл (коэффициенты Седвига)  |  28  | 62  | 130  | 289  | 611  | 930  | 1161  | 1482  | 1733   | 1995  | 2573  |

## Выводы
Классический алгоритм вставками работает очень медленно, катастрофически проигрывая алгоритму Шелла. На 70000 элементов разница более чем в 160 раз.
Вероятно, что несмотря на схожую сложность O(N^2), алгоритм вставками имеет намного худшие коэффициенты, чем алгоритм Шелла, из-за чего и проигрывает в скорости.

Сравнивая алгоритм Шелла на разных коэффициентах было замечено, что:
  - **коэффициенты Шелла** являются наименее эффективными из рассмотренных;
  - **коэффициентры Хиббарда** работают несколько быстрее, чем коэффициенты Шелла, но незначительно;
  - **коэффициенты Седвига** стабильно показывает существенно лучшие результаты чем два предыдущих варианта.
