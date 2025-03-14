# biCycle v1.4
Называется велосипедом не просто так ¯\\_(ツ)_/¯
	
# Назначение:
Header-only библиотека biCycle предоставляет возможность "запоминать" последовательности, оформленные определенным образом и описывающие пользовательские скриптовки, и вызывать их в каждом цикле пользовательской программы до тех пор, пока они не вернут true.

# Терминология
*Последовательность* 	- это функция, описывающая набор каких-либо временнО- или цикло-ориентированных событий.

*Секвенсор* 			- класс, содержащий всё необходимое для работы механизма подмешивания последовательности в основной цикл программы

# Способ применения:
1) Подключить данный заголовочный файл:

        #include "biCycle.h"
  
2) В начале программы описать пользовательскую структуру *yourStruct* и **обязательно(!)** объявить #define BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE на ваш тип:

        struct yourStruct
        {
            int a = 0;
            bool flag = false;
            /* some default variables */
        };
       #define BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE yourStruct

3) После подключения создать экземпляр класса **biCycleSequencer** с удобным именем, например - "Flow", инициализированный пользовательской структурой *yourStruct* aka **BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE** и передаваемой также в виде аргумента в хранимую функцию:

        biCycleSequencer<BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE> Flow;

4) В нужном месте, например - в главном цикле, прописать вызов метода Execute():

        while (1)
        {
          Flow.Execute();
        }
		
Этих 4 шагов достаточно для интеграции Секвенсора в проект.
	
# Занесение функций в секвенсор
Подмешивание последовательностей в основной поток программы происходит с помощью вызова метода **CallSequence()**, в аргумент которого передается структура типа **biCycleSequenceWrapper**, инициализированная пользовательской структурой *yourStruct*, содержащая конфигурацию подмешиваемой последовательности - функторы основной функции, колбеков начала и конца, а также
сам экземпляр пользовательской структуры *yourStruct*:

    biCycleSequenceWrapper<BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE> sequenceParameters;

В данном примере структура конфигурации последовательности пуста, тем не менее, занесем сконфигурированную последовательность в секвенсор:

    Flow.CallSequence( std::move(sequenceParameters) );		

Теперь в следующем цикле программы секвенсор попытается выполнить запомненную последовательность, но, так как она пуста, она сразу же отвяжется от Секвенсора и все вернется на круги своя.

Более подробные инструкции приведены в самом файле *biCycle.h*

Пример использования приведен в файле *biCycleExample.cpp*

# Актуальная версия 1.4
