# Akinator
This is my realization of akinator game ([example can be found here](https://en.akinator.com/)). My game interface is in Russian, but it can parse english words too :)!

It takes file with tree (example you can see [here](https://github.com/ThreadJava800/Akinator/blob/main/aki.txt)).

But you don't have to add all objects via file! You can add it while playing! Just in the end say that akinator guessed incorrect: program will ask you to add new node.

Here's example of gameplay (class.txt file):

> Введите номер комманды: 1

> мальчик? (да/нет)

> да

> аргентинец (да/нет)

> да

> Это Максим Платцер? (да/нет)

> нет

> Ладно, ты победил. Кто это был?

> Скажи пж: Анатолий Вассерман

> Чем Анатолий Вассерман отличается от Максим Платцер?

> много карманов

> Отлично, теперь я чуточку умнее :)

I also use graphviz library to visualize tree and festival to voice messages.

My app uses some of my previous projects: [stack](https://github.com/ThreadJava800/Secured-Stack) and [binary tree](https://github.com/ThreadJava800/BinaryTree).

My game also can give definitions to objects and compare them.