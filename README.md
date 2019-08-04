# Homework #11: Kosaraja Algorithm

На вход программе подаётся файл вроде такого ([example.txt](https://github.com/ziminas1990/otus-algorithms/blob/hw-12-Kosaraio/example.txt)):
```
A B
B E F C
C D G
D C H
E A F
F G
G F
H D G
```

В каждой строчке первый элемент - имя некоторого узла, а последующие - это перечисление всех смежных с ним узлов.

На выходе программа выдаёт описание графа в формате [graphviz](https://www.graphviz.org/), которое можно преобразовать в наглядную картинку с помощью, например, таких утилит как dot или neato (можно сделать это online на [Graphviz Online](https://dreampuf.github.io/GraphvizOnline)).

Например, для приведённого выше описания исходного графа программа сгенерирует следующий текст:
```
digraph G {
    
  graph [rankdir="LR"]
    
  0 [label = "A", color = "#D4668B", style = filled];
  1 [label = "B", color = "#D4668B", style = filled];
  2 [label = "E", color = "#D4668B", style = filled];
  3 [label = "F", color = "#90E3CF", style = filled];
  4 [label = "C", color = "#539ADF", style = filled];
  5 [label = "D", color = "#539ADF", style = filled];
  6 [label = "G", color = "#90E3CF", style = filled];
  7 [label = "H", color = "#539ADF", style = filled];


  0 -> 1;
  1 -> 2, 3, 4;
  2 -> 0, 3;
  3 -> 6;
  4 -> 5, 6;
  5 -> 4, 7;
  6 -> 3;
  7 -> 5, 6;
}
```

И в виде изображения: [example.svg](https://github.com/ziminas1990/otus-algorithms/blob/hw-12-Kosaraio/example.svg)

Задача приложения - найти в исходном графе компоненты сильной связности - т.е. такие подграфы, все элементы которых сильно связаны между собой (из любого узла можно дойти до любого другого узла подграфа). Каждому найденному подграфу назначается некоторый случайный цвет и все узлы данного подграфа раскрашиваются в этот цвет.

Более интересный пример:  
Входные данные: [example_2.txt](https://github.com/ziminas1990/otus-algorithms/blob/hw-12-Kosaraio/example_2.txt)  
Результат (уже в виде изображения): [example_2.svg](https://github.com/ziminas1990/otus-algorithms/blob/hw-12-Kosaraio/example_2.svg)  
