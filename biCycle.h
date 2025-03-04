#pragma once
#ifndef BI_CYCLE_H
#define BI_CYCLE_H


#include <iostream>
#include <vector>
#include <functional>
//#include "nameof.hpp" // подключить при необходимости ( https://github.com/Neargye/nameof )

#define BICYCLE_LIBRARY


/*
	2025-03-04  indoostrialniy  <pawel.iv.2016@yandex.ru>
	
	Описание библиотеки biCycle v1.4
	Называется велосипедом не просто так ¯\_(ツ)_/¯
	
		Назначение:
	
	Заголовочный файл "biCycle.h" описывает класс biCycleSequencer, который позволяет "запоминать" функции, отвечающие некоторым требованиям и
	описывающие пользовательские скриптовки, и обращаться к ним в каждом цикле пользовательской программы до тех пор, пока они не вернут true.
	В качестве аргумента в функцию-метод передается некий пользовательский объект.
	Имеет смысл в качестве типа описывать структуру даннных, в которую, например, по желанию можно добавлять булев флаг о завершении или вспомогательную int-переменную (см. Способ применения п.4).
	Кастомная структура данных позволит при дальнейшем масштабировании программы не перелопачивать весь код, добавляя аргументы. Своего рода - задел на будущее.
	 
	Ко всем обозначениям добавлена приставка biCycle для минимизации вероятности конфликта имен в проекте, использующем множество библиотек.
	 
	Способ применения:
	1)	Подключить данный заголовочный файл:
		#include "biCycle.h"
	
	2)	В начале программы описать тип кастомного аргумента (yourType) и ОБЯЗАТЕЛЬНО объявить #define BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE на ваш тип:
		#define BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE yourType
	
	2)	После подключения создать экземпляр класса biCycleSequencer с удобным именем, в данном примере - "Flow", с инициализацией шаблона пользовательским типом передаваемого в хранимую функцию аргумента:
		biCycleSequencer<BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE> Flow;
	
	3)	В нужном пользователю месте, вероятнее всего - в главной петле, прописать вызов метода Execute():
		
		void mainLoop()
		{
			while (!glfwWindowShouldClose(window))
			{
				Flow.Execute();
			
				glfwPollEvents();
				glfwSwapBuffers(window);
			}
		}
		Этих 3 шагов достаточно для интеграции Секвенсора в проект.
	
	4)	Подмешивание последовательностей в основной поток программы происходит с помощью вызова метода CallSequence(), 
			в аргумент которого передается структура biCycleSequenceWrapper обертки подмешиваемой функции либо
			в аргумент записываются сразу компоненты обертки через фигурные скобки. (См.описание обертки). Возможные варианты:
			
			// способ 1
			std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> startFunction = std::bind( &Body::testFunc, body);
			biCycleSequenceData datas(&completeFlag, auxVar);
			Flow.CallSequence( { std::move(startFunction), std::move(datas) } );								
			
			// способ 2
			Flow.CallSequence( { std::bind( &Body::testFunc, body), biCycleSequenceData(&completeFlag, auxVar) } );	
			
			// способ 3
			Flow.CallSequence( { std::bind( &Body::testFunc, body), biCycleSequenceData() } );	
			
			// способ 4
			Flow.CallSequence( { std::bind( &Body::testFunc, body) } );
	
	
	Описание работы:
	Для реализации своего назначения библиотека содержит:
	1) необходимую для правильной работы обертку biCycleSequenceWrapper функции, чье выполнение мы хотим подмешать в основной цикл программы
	
	3) класс biCycleSequencer, содержащий:	
		3.1) вектор sequences, хранящий все приписанные обертки
		3.2) метод CallSequence(), позволяющий добавить последовательность в вектор исполняемых sequences
		3.3) метод Execute(), чей вызов ОБЯЗАТЕЛЬНО надо вызывать в определенном пользователем месте основного потока программы. 
			Это единственный ОБЯЗАТЕЛЬНЫЙ "мостик" между Секвенсором и основной программой

	Правильно занесенная в Секвенсор последовательность будет исполняться до тех пор, пока не вернет true.
	Если не прописать возврат true, подмешанная последовательность будет исполняться вплоть до закрытия программы.
	Т.о. можно удобно реализовывать некоторые скрипты по принципу "вызвал исполнение - и забил". Например, прописать т.о. действия, выполнение которых не требуется в каждом цикле, а лишь с определенным интервалом.
 
	Терминология: 
	последовательность 	- это функция, описывающая набор каких-либо временнО- или цикло-ориентированных событий 
	Секвенсор 			- класс, содержащий всё необходимое для работы механизма подмешивания последовательности в основной цикл программы


	Функция, которую собираемся подмешать в поток команд цикла, имеет 4 аргумента - структуру со вспомогательными параметрами,  также "коллбек"-функции начала и конца,
	и должна возвращать булеву переменную. Такова задумка.


	Шаблон, которого должна придерживаться пользовательская последовательность (функция/метод).
	
	Основные требования: "шапка"
			bool yourOwnSequenceName(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE& data)
	и, соответственно, обязательное возвращение bool в теле функции.
	
	Возвращенный true прекращает дальнейшее исполнение последовательности (смотри функцию Execute() )
	

	Обертка:
	
	Структура SequenceWrapper - это "обертка" для подмешиваемой последовательности, она хранит: 
	адрес функции; 
	структуру с данными аргументов ( указатель на флаг завершенности функции (использующийся для сигнализации о выполнении последовательности), вспомогательная целочисленная переменная-аргумент и т.д.
*/


// Структура хранит необходимые данные для корректного занесения последовательности в секвенсор
template <typename S>		
struct biCycleSequenceWrapper
{	
	std::function<bool(S&)> 	function;
	S							data ;
	
	std::function<bool(S&)>		startFunc;								// с аргументами
	std::function<bool(S&)>		endFunc;								
	
	std::function<void()>		start_callback;							// без аргументов
	std::function<void()>		end_callback;
};



template <typename S>
class biCycleSequencer
{
	std::vector <biCycleSequenceWrapper<S> > sequences;					//вектор с обертками функций

public:
	// функция CallSequence() лишь принимает ссылочную оберточную структуру biCycleSequenceWrapper функции и сохраняет ее в приватном векторе sequences
	void CallSequence( const biCycleSequenceWrapper<S>& newSequence)
	{
		sequences.push_back( std::move(newSequence) );	
		
		// так как вносим в конец вектора новую последовательность, которую подразумевается сразу же начать исполнять, то для вызова начального колбека
		// получим последний элемент вектора функцией .back(), которая и будет нашей вносимой
		
		if(sequences.back().startFunc)
		{	sequences.back().startFunc(sequences.back().data);	}		// вызов стартовой функции!!!
		
		if(sequences.back().start_callback)
		{ sequences.back().start_callback();	}
		
	}

	// функция Execute() должна вызываться из основной части программы. Например, можно вызывать ее в каждом цикле главной петли
	void Execute()
	{
		if( !sequences.empty() )
		{
			auto it = sequences.begin();
			
			while (it != sequences.end()) 
			{
				if(it->function)
				{
					bool result = it->function(it->data);
				
					if (result) 
					{
						if (it->endFunc) 		it->endFunc(it->data);		
						if (it->end_callback) 	it->end_callback();
						
						it = sequences.erase(it); // erase возвращает следующий итератор
					} 
					else 
					{
						++it;
					}
				}
				else // если функция вдруг оказалась пустой
				{
					//std::cout << "\t\tEmpty main func!\n";
					
					if (it->endFunc) 		it->endFunc(it->data);
					if (it->end_callback) 	it->end_callback();
					
					it = sequences.erase(it); // erase возвращает следующий итератор
				}
				
			}
		}
	}

	~biCycleSequencer()
	{
		sequences.clear(); //очистка вектора последовательностей
	}
};


#endif
