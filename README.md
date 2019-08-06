# Homework #13: Demucron Algorithm
## Пример работы
Граф ДО топологической сортировки: [example_before.svg](https://github.com/ziminas1990/otus-algorithms/blob/hw-13-Demucron/example_before.svg)  
Граф ПОСЛЕ топологический сортировки: [example_after.svg](https://github.com/ziminas1990/otus-algorithms/blob/hw-13-Demucron/example_after.svg)

## Описание приложения
Приложение имеет 2 необязательных параметра:
```
Demucron [<file>] [dot]
```
  - <file> - файл с описанием графа в формате, предусмотренным заданием;
  - dot - если параметр указан (его значение совпадает с его именем), то приложение выдаст на выход не список уровней (как предусмотрено заданием), а описание графа в формате GraphViz с указанием уровней узлов
  
Пусть есть следующий входной файл **в формате, описанным в задании** (example.txt):
```
5 2 6
6 8 7
1
5 2 7
0 1
1
8
8
```
Этому файлу соответствует следующий граф: [example_before.svg](https://github.com/ziminas1990/otus-algorithms/blob/hw-13-Demucron/example_before.svg)

Если запустим приложение, указав только файл, то получим список уровней (первый столбец) и список узлов, на каждом уровне. Т.е. вывод **соответствует требованию задания**.
```
$ ./Demucron ./example.txt
```
```
0	 3 4
1	 0
2	 5 2
3	 1
4	 6 7
5	 8
```

Если запустим приложение, указав не только файл, но и дополнительную опцию "dot", то приложение сгенерит граф в формате GraphViz:
```
./Demucron ./example.txt dot
```
```
digraph G {
  0 [label = "0", color = "#A0A0A0", style = filled];
  1 [label = "5", color = "#A0A0A0", style = filled];
  2 [label = "2", color = "#A0A0A0", style = filled];
  3 [label = "6", color = "#A0A0A0", style = filled];
  4 [label = "1", color = "#A0A0A0", style = filled];
  5 [label = "8", color = "#A0A0A0", style = filled];
  6 [label = "7", color = "#A0A0A0", style = filled];
  7 [label = "3", color = "#A0A0A0", style = filled];
  8 [label = "4", color = "#A0A0A0", style = filled];

  { rank = same; 7; 8; }
  { rank = same; 0; }
  { rank = same; 1; 2; }
  { rank = same; 4; }
  { rank = same; 3; 6; }
  { rank = same; 5; }

  0 -> 1, 2, 3;
  1 -> 4;
  2 -> 4;
  3 -> 5;
  4 -> 3, 5, 6;
  5;
  6 -> 5;
  7 -> 1, 2, 6;
  8 -> 0, 4;
}
```

Текст в формате graphviz можно преобразовать в картинку графа используя, утилиту **dot**. Например, на сайте [GraphViz Online](https://dreampuf.github.io/GraphvizOnline).  
Выданное приложением описание графа преобразуется в [граф после топологический сортировки (example_after.svg)](https://github.com/ziminas1990/otus-algorithms/blob/hw-13-Demucron/example_after.svg)
