# Homework #17: Heap Manager
## Описание приложения
В рамках задачи был реализован класс **AwesomeHeap**. Класс реализует управление кучей: аллокацию и освобождение участков памяти произвольного размера.  
При создании объекта класса нужно указать:
  - размер кучи;
  - политику поиска подходящего фрагмента.
  
Всего AwesomeHeap поддерживает 3 разных политики:
  - **eFirst** - использует первый найденный фрагмент памяти подходящего размера;
  - **eBest** - использует самый маленький фрагмент памяти, вмещающий указанный объём данных;
  - **eWorst** - использует самый большой фрагмент памяти.

Менеджер реализован следующим образом:\\
изначально куча представляет из себя один большой (размером со всю кучу) свободный Chunk. В заголовке Chunk'а распологается два указателя: на следующий Chunk и на предыдущий Chunk. Но это **не совсем** связный список, т.к. Chunk'и располагаются в памяти непосредственно друг за другом.  
Когда требуется **выделить** некоторый объём памяти из кучи, менеджер выполняет следующие действия:
  1. ищет подходящий Chunk (согласно указанной политике);
  2. помечает chunk как занятый;
  3. отступает от чанка требуемое количество байт (выравненное по 8) и на оставшейся от исходного chunk'а памяти создаёт новый свободный chunk.
Когда требуется **освободить** некоторый участок памяти, менеджер выполняет следующие действия:
  1. получает указатель на chunk, которому принадлежит память (за O(1));
  2. помечает chunk как свободный;
  3. если chunk, следующий за освобождённым, помечен как свободный, то присоединяется к текущему чанку;
  4. если предыдущий chunk помечен как свободный, то текущий чанк присоединяется к нему.
  
 Таким образом, если в куче никогда не будет существовать два последовательно идущих друг за другом свободных чанка - они будут объединятся в один, более крупный.
 
 ## Тестирование работы AweomeHeap
 Для проверки корректности работы AwesomeHeap в main.cpp был реалиован тест **Test()**. Подробнее о его логике напишу здесь завтра.
 
 ## Сравнение фрагментации и производительности для разных политик
 На базе теста Test() было проведено сравнение фрагментации и производительности AwesomeHeap при использовании разных политик.
 Результаты теста опубликовал в виде красивых графиков в google docs: [HW-17-Heap-Manager](https://docs.google.com/spreadsheets/d/19XK6a7a0GRHI84NOxD0WsdQVqNiVFSQOYbg3qVRnCwg/edit?usp=sharing) (графики загружаются не сразу)  
 Подробнее о методике оценки степени фрагментации я напишу завтра.

