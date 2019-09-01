#  ДЗ #21: LRU Cache
## Что сделано?
В рамках домашней работы был реализован LRU Cache, позволяющий кэшировать работу с памятью.
В приложении имитируется некоторая абстрактная память (класс **AbstractRAM**), для которой можно указать скорость работы на чтение и запись. Скорость измеряется в тиках (ticks) - это условные единицы.
Для ясности приведу интерфейс AbstractRAM:
```
class AbstractRAM
{
public:
  virtual ~AbstractRAM() = default;

  virtual uint8_t get(size_t addr) = 0;
  virtual void    set(size_t addr, uint8_t value) = 0;
  virtual void    load(size_t addr, uint8_t* pDest, size_t nBusWidth) = 0;
  virtual void    flush(size_t addr, uint8_t const* pSource, size_t nBusWidth) = 0;
  virtual size_t  lastOperationTime() const = 0;
};
```
Кроме функций **get()** и **set()**, символизирующих чтение/запись в память, AbstractRAM так же реализует функции **load()** и **flush()**. Эти функции позволяют вычитать из памяти не отдельный элемент, а сразу последовательное множество элементов **за одну операцию** чтения или записи. Такие операции символизируют чтение/запись в память с использованием всей ширины системной шины. Это критично важно для кэширования, т.к. позволяет максимально эффективно использовать системную шину, выгружая кэш-линию целиком за одну операцию (о кэш-линиях я напишу ниже).

Класс AbstractRAM имеет два наследника - RAM и LRUCache.  
Первый - **RAM** - это простая обёртка над массивом и выступает в роли "медленной" оперативной памяти.  
Второй - **LRUCache** реализует поверх переданного ему наследника AbstractRAM логику LRU-кэширования.

В приложении реализован набор тестов, позволяющий измерить производительность работы с памятью в разных режимах (произвольный доступ, частично произвольный доступ, последовательный доступ) в разными размерами кэшей.

## Как устроен LRU-кэш?
Идея такая - кэш состоит из множества **кэш-линий**, все кэш-линии имеют одинаковый (относительно небольшой) размер, причём подразумевается, что **размер кэш-линии равен размеру системной шины**. Иначе говоря, кэш-линию можно загрузить из памяти (или записать в память) за одну операцию чтения или записи.

При обращении к адресу в памяти, который ещё не загружен в кэш, из памяти считывается блок, равный размеру кэш-линии, причём адрес начала блока **выравнивается** по размеру кэш-линии.

LRU-кэш состоит из двух структур данных:
  - **список кэш-линий** - кэш-линии в списке **отсортированы** в порядке времени последнего времени доступа к ним, т.е. в начале списка находится кэш-линия, над которой была проведена **последняя операция чтения**;
  - **хэш-таблица кэш-линий** - позволяет по адресу памяти (выравненному по размеру кжш-линии) получить кшэ-линию, в которую адрес был загружен, если таковая имеется.

**Список** используется для того, чтобы можно было за **О(1)** найти самую "непопулярную" кэш-линию. Именно такая кэш-линия будет вытеснена при загрузке новой кэш-линии.  
**Хэш-таблица** используется для того, чтобы за **О(1)** найти в кэше элемент с определённым адресом, если он был загружен в кэш.

**Операция чтения** реализована следующим образом:
1. через обращение к хэш-табице находим кэш-линию, которая содержит требуемый элемент;
2. если кэш-линия найдена, то она переносится **в начало списка** кэш-линий и элемент возвращается клиенту;
3. если кэш-линия отсутствует, то:
4.1 . забираем **последнюю кэш-линию** из списка; если в кэш-линию вносились изменения (через операцию записи), то она **сбрасывается** в память (операцией flush);
4.2 загружаем в кэш-линию тот участок памяти, выровненный по размеру кэш-линии, где находится требуемый элемент;
4.3 размещаем кэш-линию **в начало списка** и возвращаемй клиенту требуемый элемент.


**Операция записи** реализована следующим образом:
1. через обращение к хэш-табице находим кэш-линию, которая содержит требуемый элемент;
2. если кэш-линия **найдена**, то в неё записывается новое значение соответствующего элемента и она **помечается как модифицированная**;
3. если кэш-линия **не найдена**, то операция просто перенаправляется в память.

Обращаю внимание, что операция записи **не изменяет** последнее время доступа к кэш-линии.

За счёт того, что LRUCache реализует интерфейс AbstractRAM и в качестве нижнего уровня так же принимает указатель на AbstractRAM, можно построить **иерархиею кэшей**, как в микропроцессорах. Но делать такой тест производительности для такой схемы я не стал, хоть было бы и интересно.

## Сравнение производительности
Если запустить приложение с единственным аргументом "perfomance", то приложение прогонит набор тестов производительности и выведет на экран три таблицы:
  1. **случайное чтение/запись по всей памяти**
  2. **частично-случайное чтение/запись**
  3. **сканирование случайных участков памяти**

Пример вывода приложения (столбцы выровнены вручную):
```
./Caching perfomance
Random R/W test:
8   2079 2080 2078 2078 2078 2073
16  2079 2079 2078 2077 2075 2068
32  2079 2079 2076 2073 2069 2055
64  2078 2077 2073 2068 2055 2030
128 2076 2074 2067 2055 2030 1980
256 2073 2068 2056 2030 1980 1881

Partially random R/W test:
8   2068 2058 2036 1992 1907 1718
16  2057 2034 1987 1894 1706 1307
32  2033 1985 1894 1705 1321 442
64  1989 1897 1720 1346 581  347
128 1914 1748 1422 832  551  341
256 1803 1545 1158 826  534  324

Random scanning R/W test:
8   1309 822 643 546 534 427
16  1309 822 643 546 534 427
32  1309 822 643 546 534 426
64  1309 822 643 546 533 426
128 1309 822 643 526 532 416
256 1308 817 643 520 523 410
```
Первый столбец - это длина кэш-линии.  
По данным таблицам были составлены графики в Googl Docs: [HW-21-LRUCache](https://docs.google.com/spreadsheets/d/1d4PVBSZdH3NHdCy59gA4QK8LDQ520zK5I9hHwfNm7_U/edit?usp=sharing). Для каждой таблицы график расположен на отдельной странице документа.

Строки таблицы - длина одной кэш-линии в байтах, от 2 до 64 байт .
Столбцы таблицы - общее количество кэш-лайнов в кэше (от 8 до 256)

Все тесты проводятся на **следующих исходных данных**:
  1. **128 * 1024 байт** - размер виртуальной оперативной памяти;
  2. в каждом тесте для каждого кэша производится 100000 операций, причём 25% из них - операции записи, остальные - операции чтения.
  3. размер кэш линий варьируется от 2 до 64;
  4. количество кэш-линий в кэше варьируется от 8 до 256.

Таким образом, **минимальный рзмер кэша** - 2 * 8, т.е. 16 байт, или **0.006%** от общего объёма памяти, а **максимальный размер кэша** - 64 * 256, т.е. 16 КБайт, или **12.5%** от общего объёма оперативной памяти.

В общем виде тест выглядит так:
1. перебираются все возможные пары длины кэш-линии и количества страниц;
2. для каждой пары создаётся LRU-кэш
3. для каждого кэша выполняется 100.000 операций чтения/записи
4. измеряется количество тиков, которое потребовалось для выполнения операций из п.3

### Произвольное чтение и запись
В данном тесте операции чтения и записи производились случайно по всей памяти.
