# Homework #13: Demucron Algorithm
## Описание приложения
Приложение имеет 2 необязательных параметра:
```
Demucron [<file>] [dot]
```
  - <file> - файл с описанием графа в формате, предусмотренным заданием;
  - dot - если параметр указан (его значение совпадает с его именем), то приложение выдаст на выход не список уровней (как предусмотрено заданием), а описание графа в формате GraphViz с указанием уровней узлов
  
Пусть есть следующий входной файл **в формате, описанным в задании** (example.txt):
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
Если запустим приложение, указав только файл, то получим <...>. Т.е. вывод **соответствует требованию задания**.
```
$ ./Kruskal ./example.txt
```
```
0 -- 1
1 -- 4
1 -- 5
2 -- 4
2 -- 6
3 -- 6
4 -- 7
5 -- 8
```

Если запустим приложение, указав не только файл, но и дополнительную опцию "dot", то приложение сгенерит граф в формате GraphViz:
```
./Kruskal ./example.txt dot
```
```
digraph G {
  0 -> 1 [dir = none, label = 1, len = 1, color = "#FF0000"]
  0 -> 2 [dir = none, label = 3, len = 3, color = "#000000"]
  0 -> 3 [dir = none, label = 4, len = 4, color = "#000000"]
  0 -> 6 [dir = none, label = 4, len = 4, color = "#000000"]
  1 -> 4 [dir = none, label = 2, len = 2, color = "#FF0000"]
  1 -> 5 [dir = none, label = 2, len = 2, color = "#FF0000"]
  1 -> 8 [dir = none, label = 4, len = 4, color = "#000000"]
  2 -> 3 [dir = none, label = 5, len = 5, color = "#000000"]
  2 -> 4 [dir = none, label = 2, len = 2, color = "#FF0000"]
  2 -> 6 [dir = none, label = 2, len = 2, color = "#FF0000"]
  2 -> 7 [dir = none, label = 4, len = 4, color = "#000000"]
  3 -> 6 [dir = none, label = 2, len = 2, color = "#FF0000"]
  4 -> 5 [dir = none, label = 3, len = 3, color = "#000000"]
  4 -> 7 [dir = none, label = 3, len = 3, color = "#FF0000"]
  5 -> 8 [dir = none, label = 3, len = 3, color = "#FF0000"]
  7 -> 8 [dir = none, label = 3, len = 3, color = "#000000"]
}
```

Текст в формате graphviz можно преобразовать в картинку графа используя, утилиту **neato**. Например, на сайте [GraphViz Online](https://dreampuf.github.io/GraphvizOnline).
Выданное приложением описание графа преобразуется в [example.svg](https://github.com/ziminas1990/otus-algorithms/blob/hw-14-Kruskal/example.svg)
