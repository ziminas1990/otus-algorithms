# Homework #13: Demucron Algorithm
## Описание приложения
Приложение находит на указанном графе минимальный маршрут от узла A к узлу B, используя алгоритм Дейкстры.  
Если запустить приложение **без параметров**, то оно сгенерит произвольный граф и найдёт на нём кратчайший путь между двумя произвольными узлами. Рёбра кратчайшего пути на графе окрашены в красный цвет. Примеры работы на сгенерированных графах:
  - [граф на 16 узлов](https://github.com/ziminas1990/otus-algorithms/blob/hw-15-Deikstra/example_16.svg)
  - [граф на 32 узлов](https://github.com/ziminas1990/otus-algorithms/blob/hw-15-Deikstra/example_32.svg)
  - [граф на 64 узлов](https://github.com/ziminas1990/otus-algorithms/blob/hw-15-Deikstra/example_64.svg)
  - [граф на 128 узлов](https://github.com/ziminas1990/otus-algorithms/blob/hw-15-Deikstra/example_128.svg)
  - [граф на 256 узлов](https://github.com/ziminas1990/otus-algorithms/blob/hw-15-Deikstra/example_256.svg)

Для запуска приложения на заранее составленно графе, необходимо передать 3 обязательных и один опциональный параметр:
```
Demucron <file> start finish [dot]
```
  - <file> - файл с описанием графа в формате, предусмотренным заданием;
  - start - номер узла, который будет начальной точкой маршрута;
  - finish - номер узла, который будет конечной точкой маршрута;
  - dot - если параметр указан (его значение совпадает с его именем), то приложение выдаст на выход не список вершин, составляющих путь, а граф в формате graphviz, где рёбра пути окрашены в краынй цвет.
  
Пусть есть следующий входной файл **в формате, описанным в задании** ([https://github.com/ziminas1990/otus-algorithms/blob/hw-15-Deikstra/example.svg]()):
```
0 1 3 4 0 0 4 0 0
1 0 0 0 2 2 0 0 4
3 0 0 5 2 0 2 4 0
4 0 5 0 0 0 2 0 0
0 2 2 0 0 3 0 3 0
0 2 0 0 3 0 0 0 3
4 0 2 2 0 0 0 0 0
0 0 4 0 3 0 0 0 3
0 4 0 0 0 3 0 3 0
```
Если запустим приложение, указав файл, а так же начальную и конечную точку маршрута, то приложение выведет список вершин, составляющих наиболее короткий машршрут. Т.е. вывод **соответствует требованию задания**.
```
$ ./Deikstra ./example.txt 3 7
```
```
3 -> 6 -> 2 -> 7
```

Если запустим приложение, указав дополнительную опцию "dot", то приложение сгенерит граф в формате GraphViz:
```
./Deikstra ./example.txt 3 7 dot
```
```
digraph G {
  0 -> 1 [dir = none, label = "~10", len = 1, color = "#000000"]
  0 -> 2 [dir = none, label = "~30", len = 3, color = "#000000"]
  0 -> 3 [dir = none, label = "~40", len = 4, color = "#000000"]
  0 -> 6 [dir = none, label = "~40", len = 4, color = "#000000"]
  1 -> 4 [dir = none, label = "~20", len = 2, color = "#000000"]
  1 -> 5 [dir = none, label = "~20", len = 2, color = "#000000"]
  1 -> 8 [dir = none, label = "~40", len = 4, color = "#000000"]
  2 -> 3 [dir = none, label = "~50", len = 5, color = "#000000"]
  2 -> 4 [dir = none, label = "~20", len = 2, color = "#000000"]
  2 -> 6 [dir = none, label = "~20", len = 2, color = "#FF0000"]
  2 -> 7 [dir = none, label = "~40", len = 4, color = "#FF0000"]
  3 -> 6 [dir = none, label = "~20", len = 2, color = "#FF0000"]
  4 -> 5 [dir = none, label = "~30", len = 3, color = "#000000"]
  4 -> 7 [dir = none, label = "~30", len = 3, color = "#000000"]
  5 -> 8 [dir = none, label = "~30", len = 3, color = "#000000"]
  7 -> 8 [dir = none, label = "~30", len = 3, color = "#000000"]
}
```

Текст в формате graphviz можно преобразовать в картинку графа используя, утилиту **neato** или **fdp**. Например, на сайте [GraphViz Online](https://dreampuf.github.io/GraphvizOnline).
Выданное приложением описание графа преобразуется в [example.svg](https://github.com/ziminas1990/otus-algorithms/blob/hw-15-Deikstra/example.svg)
