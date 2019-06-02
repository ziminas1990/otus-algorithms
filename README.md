# ДЗ #3: алгебраические алгоритмы
В рамках домашнего задания реализовано 4 приложения. Каждое приложений позволяет указать вариант используемого алгоритма и входные данные. Приложение выводит на экран результат работы алгоритма и время его выполнения.
Если запустить приложение без аргументов, будет выведена справка и примеры.

  * **Euklid.cpp** - реализация двух алгоритмов Евклида (через вычитание и через остаток от деления)
  * **Power.cpp** - реализация алгоритм возведения в степень:
	  * **power_naive()** - наивная реализация через N перемножений;
	  * **power_optimized()** - оптимизация наивного алгоритма;
	  * **power_best()** - быстрый алгоритм возведение в степень;
  * **Primes.cpp** - реализация алгоритмов поиска всех простых чисел до N:
	  * **primes_naive()** - наивная реализация с перебором всех множителей (до первого делителя);
	  * **primes_oprimized()** - оптимизация наивной реализации: перебор делителей осуществляется только по чётным числам и только до квадратного корня от исходного числа;
	  * **primes_eratosfen()** - реализация через решето Эратосфена; для хранения массива был реализован битовый массив Bitsarray, который позволяет в одном байте хранить 8 флагов;
  * **Fibonacci.cpp** - реализация алгоритмов вычисления чисел Фибоначи;
	  * **fibonacci_recursive()** - реализация через рекурсию;
	  * **fibonacci_iterative()** - реализация через итерацию;
	  * **fibonacci_gold()** - вычисление через рекурентное соотношение.


