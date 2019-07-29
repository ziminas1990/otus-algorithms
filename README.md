# Homework #10: Searching in splay-tree

## Сравнение производительности Splay и AVL-дерева
Все тесты проводились на массиве из 300.000 элементов.

### Как генерируется массив входных данных?
В описанных ниже сценариях есть пункт "Генерим массив X". Для генерации массива передаётся два параметра: длина массива Size и размеры сортируемых блоков N. Массив генерируется следующим образом:
1. массив заполняется значениями от 0 до Size-1;
2. массив разбивается на несколько блоков, каждый имеет длину N; последний блок может быть больше чем N, но меньше чем 2N;
3. элементы в каждом блоке перемешиваются между собой в случайном порядке.

Например:
Если N = 1, то перемешивается весь массив целиком.
Если N = 2, то независимо перемешиваются левая и правая части массива.
Если N больше чем половина длины массива, то массив остаётся отсортированным (т.к. каждый блок состоит из одного элемента, а значит он уже отсортирован).

Пример:

| N               | Массив после перемешивания         |
|-----------------|------------------------------------|
| Исходный массив | 1 2 3 4 5 6 7 8 9                  |
| N = 1           | 7 3 8 1 2 4 9 5 6                  |
| N = 2           | 2 1 3 5 4 - 7 8 6 9                |
| N = 3           | 3 1 2 - 6 5 4 - 7 9 8              |
| N = 4           | 2 1 - 3 4 - 6 5 - 7 8  9           |
| N >= 5          | 1 - 2 - 3 - 4 - 5 - 6 - 7 - 8 - 9  |

**В таблицах ниже, число в заголовке каждого столбца - это количество сортируемых блоков, т.е. N.**

**Вставка элементов:**

Сценарий теста:
1. Генерим массив A
2. Засекаем время и вставляем все элементы массива A в дерево

| Количество блоков |  1  |  2  |  4  |  16 | 64  | 256 | 512 | 1024 | 2048 | 4096 |
|-------------------|-----|-----|-----|-----|-----|-----|-----|------|------|------|
| Splay Tree        | 249 | 204 | 171 | 137 | 108 | 95  | 78  | 77   | 61   | 56   |
| AVL Tree          | 271 | 237 | 206 | 185 | 176 | 175 | 169 | 168  | 166  | 163  |

**Вывод:**
Вставка в splay-дерево работает **быстрее**, чем вставка в AVL-дерево, причём чем лучше отсортированы выходные данные, тем быстрее происходит вставка. Уменьшение времени в зависимости от сортировки данных можно объяснить тем, что на отсортированных данных вероятность того, что каждй следующий элемент будет вставлен не глубоко в дереве, повышается, а значит придётся делать меньше операций поворота.

Вставка в AVL-дерево работает медленнее, но на более отсортированных данных она осуществляется быстрее. Думаю, это можно объяснить тем, что чем лучше отсортированы данные, тем выше вероятность, что для поиска места под новый элемент будет пройден примерно один и тот же путь по дереву. Это увеличивает шанс кэшхита при обращении к узлу дерева, что увеличивает производительность.

**Удаление элементов:**

Сценарий теста:
1. Генерим массив A;
2. Вставляем все элементы массива A в дерево;
3. Генерим массив B;
4. Засекаем время и удаляем из дерева все элементы массива B;

| Количество блоков |  1  |  2  |  4  |  16 | 64  | 256 | 512 | 1024 | 2048 | 4096  |
|-------------------|-----|-----|-----|-----|-----|-----|-----|------|------|-------|
| Splay Tree        | 401 | 311 | 252 | 185 | 151 | 240 | 555 | 1566 | 4488 | 13260 |
| AVL Tree          | 286 | 235 | 193 | 162 | 141 | 132 | 124 | 125  | 114  | 111   |

**Вывод:**
Удаление случайных элементов в splay-дереве работает **значительно хуже**, чем в AVL-дереве. Причём, скорость удаления резко деградирует на более отсортированных данных. Это связано с тем, что чем лучше отсортированы данные, тем "выше" будет splay-дерево и при удалении элементов потребуется больше поворотов, чтобы поднять удаляемый элемент в корень дерева.
При полностью отсортированных данных можно получить переполнение стека из-за рекурсии.

В AVL-дереве скорость удаления увеличивается на более отсортированных данных. Скорее всего, как и в случае со вставкой, это происходит благодаря кэшированию узлов в кэше процессора и кэшхитах при прохождении по одинаковым маршрутам в дереве.

**Случайный поиск элементов:**
Сценарий теста:
1. Генерим массив A;
2. Вставляем все элементы массива A в дерево;
3. Генерим массив B;
4. Засекаем время и проверяем наличие всех элементов из массива B в дереве.

| Количество блоков |  1  |  2  |  4  |  16 | 64  | 256 | 512 | 1024 | 2048 | 4096 |
|-------------------|-----|-----|-----|-----|-----|-----|-----|------|------|------|
| Splay Tree        | 355 | 267 | 203 | 151 | 117 | 148 | 271 | 709  | 2008 | 6249 |
| AVL Tree          | 152 | 114 | 93  | 77  | 67  | 62  | 59  | 60   | 56   | 54   |

**Выводы:**
На случайном поиске Splay-дерево **проигрывает** бинарному дереву, причём чем лучше отсортированы входные данные, тем медленнее работает скорость, причём деградация стремительная. Это связано с тем, что на более отсортированных данных дерево получается выше.

Рост производительности на более отсортированных данных в AVL-дереве опять же хорошо объясняется кэшированием узлов в кэше процессора (как и в двух предыдущих случаях).

**Повторяющийся поиск небольшого набора элементов:**
Данный тест сделан для того, чтобы показать, что splay-дерево быстрее ищет элементы, которые были найдены недавно.
Тест имеет параметр PoolSize - размер пула элементов, для которых будет производится поиск.

Сценарий теста:
1. Генерим массив A;
2. Вставляем все элементы массива A в дерево;
3. Генерим массив B;
4. Создаём массив C из первых PoolSize элеменгтов массива B;
5. Поочерёдно ищем в дереве все элементы из массива C;
6. Шаг 5 повторяем множество раз (2 * размер массива / PoolSize).

Для каждого PoolSize будет своя таблица:

**PoolSize = 5**

| Количество блоков |  1  |  2  |  4  |  16 | 64  | 256 | 512 | 1024 | 2048 | 4096 |
|-------------------|-----|-----|-----|-----|-----|-----|-----|------|------|------|
| Splay Tree        | 34  | 32  | 35  | 34  | 35  | 34  | 34  | 36   | 36   | 38   |
| AVL Tree          | 81  | 83  | 74  | 78  | 82  | 70  | 85  | 86   | 79   | 65   |

**PoolSize = 10**

| Количество блоков |  1  |  2  |  4  |  16 | 64  | 256 | 512 | 1024 | 2048 | 4096 |
|-------------------|-----|-----|-----|-----|-----|-----|-----|------|------|------|
| Splay Tree        | 43  | 47  | 45  | 44  | 43  | 45  | 44  | 46   | 44   | 52   |
| AVL Tree          | 79  | 83  | 78  | 85  | 73  | 78  | 75  | 83   | 73   | 84   |


**PoolSize = 25**

| Количество блоков |  1  |  2  |  4  |  16 | 64  | 256 | 512 | 1024 | 2048 | 4096 |
|-------------------|-----|-----|-----|-----|-----|-----|-----|------|------|------|
| Splay Tree        | 61  | 61  | 59  | 64  | 58  | 60  | 61  | 64   | 59   | 63   |
| AVL Tree          | 83  | 84  | 77  | 79  | 85  | 91  | 78  | 87   | 76   | 88   |

**PoolSize = 50**

| Количество блоков |  1  |  2  |  4  |  16 | 64  | 256 | 512 | 1024 | 2048 | 4096 |
|-------------------|-----|-----|-----|-----|-----|-----|-----|------|------|------|
| Splay Tree        | 71  | 70  | 74  | 70  | 74  | 76  | 68  | 73   | 75   | 77   |
| AVL Tree          | 97  | 88  | 82  | 80  | 77  | 87  | 82  | 90   | 80   | 88   |

**Выводы:**
Для всех пяти случаев можно сделать общий вывод:
splay-дерево **выигрывает** у AVL-дерева за счёт того, что при поиске элемента, он "всплывает" ближе к корню дерева и его повторный поиск требует меньше времени. Однако, с ростом размера пула выигрыш от этого становится всё менее заметным, т.к. чем больше пул, тем глубже успевает погрузиться элемент от корня, прежде чем его поиск будет повторён вновь.
