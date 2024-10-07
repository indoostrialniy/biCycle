/*
	2024-10-07  indoostrialniy  <pawel.iv.2016@yandex.ru>
	
	Описание библиотеки biCycle v1.1
	Называется велосипедом не просто так ¯\_(ツ)_/¯
	
	Назначение:
	Заголовочный файл "biCycle.h" описывает класс biCycleSequencer, который позволяет "запоминать" функции, отвечающие некоторым требованиям и
	описывающие пользовательские скриптовки, и исполнять их в каждом цикле пользовательской программы 
	до тех пор, пока они не вернут true. По желанию можно дополнительно указывать булев флаг о завершении (см. Способ применения п.4).
	 
	Ко всем обозначениям добавлена приставка biCycle для минимизации вероятности конфликта имен в проекте, использующем множество библиотек.
	 
	Способ применения:
	1)	Подключить данный заголовочный файл:
		#include "biCycle.h"
	2)	После подключения создать экземпляр класса biCycleSequencer с удобным именем, в данном примере - "Flow":
		biCycleSequencer Flow;
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
			в аргумент которого можно передать структуру biCycleSequenceWrapper обертки подмешиваемой функции либо
			записать в аргумент сразу компоненты обертки через фигурные скобки. (См.описание обертки). Возможные варианты:
			
			а - biCycleSequenceWrapper newSequence("string_function_name_id", functionNameWithoutBrackets, ownerClassPointer);
				Flow.CallSequence( newSequence );	
			
			б - biCycleSequenceWrapper newSequence("string_function_name_id", functionNameWithoutBrackets, ownerClassPointer, &boolReportFlag);
				Flow.CallSequence( newSequence );	
			
			в - Flow.CallSequence( {"string_function_name_id", functionNameWithoutBrackets, ownerClassPointer} );	
			
			г - Flow.CallSequence( {"string_function_name_id", functionNameWithoutBrackets, ownerClassPointer, &boolReportFlag} );		
	
	
	Описание работы:
	Для реализации своего назначения библиотека содержит:
	1) необходимую для правильной работы обертку biCycleSequenceWrapper функции, чье выполнение мы хотим подмешать в основной цикл программы
	2) типоопределение biCycleMixedSequenceFunc для возможности внесения функций в обертку
	
	3) класс biCycleSequencer, содержащий:	
		3.1) вектор sequences, хранящий все приписанные обертки
		3.2) метод CallSequence(), позволяющий добавить последовательность в вектор исполняемых sequences
		3.3) метод Execute(), чей вызов ОБЯЗАТЕЛЬНО надо вызывать в определенном пользователем месте основного потока программы. 
			Это единственный ОБЯЗАТЕЛЬНЫЙ "мостик" между Секвенсором и основной программой

	Правильно занесенная в Секвенсор последовательность будет исполняться до тех пор, пока не вернет true.
	Т.о. можно удобно реализовывать некоторые скрипты по принципу "вызвал исполнение - и забил".
 
	Терминология: 
	последовательность 	- это функция, описывающая набор каких-либо временнО- или цикло-ориентированных событий 
	Секвенсор 			- класс, содержащий всё необходимое для работы механизма подмешивания последовательности в основной цикл программы
*/

#include <iostream>
#include <vector>

#define BICYCLE_LIBRARY

/*
	Функция, которую собираемся подмешать в поток команд цикла, имеет 1 аргумент (указатель на объект типа void) 
	и должна возвращать булеву переменную. Такова задумка. Обозначим указатель:
*/

typedef bool (*biCycleMixedSequenceFunc)(void*); 


/*
	Шаблон, которого должна придерживаться пользовательская последовательность (функция/метод).
	Основные требования: "шапка"
			static bool yourOwnSequenceName(void* ptr)
	и, соответственно, обязательное возвращение bool в теле функции.
	
	Возвращенный true прекращает дальнейшее исполнение последовательности (смотри функцию Execute() )
	
	ОСТОРОЖНО, КОСТЫЛЬ !
	Аргумент ptr функции yourOwnSequenceName(void* ptr) указывается как void* во избежание проблем при компиляции и во имя единообразия.
	А так как при занесении функции в Секвенсор явно указывается и адрес класса-владельца, то очевидно, что аргумент void* ptr
	всегда (ну, почти наверняка) будет являться указателем на объект такого же класса, как и класс-владелец. (Потому, что де-факто это он же и есть!)
	А значит, можно принудительно заставить функцию-последовательность воспринимать ptr как указатель на класс, которому она принадлежит, введя для удобства новый define:

		#define OWNER ((OwnerClassType*)ptr)
		
	static bool sequencerFuncTemplate(void* ptr)
	{
		//	OWNER->getOwnerClassData();
		//	аналогично
		//	((OwnerClassType*)ptr)>getOwnerClassData();
			
		
		// обязательная часть функции последовательности
		#define finish_condition_has_come 1
		if ( finish_condition_has_come )
		{
			// some code, if needed
			return true;	// возвращение true-флага отвяжет последовательность от секвенсора и она больше не будет исполняться
		}
		else
		{
			// some code, if needed
			return false;	// возвращение false-флага сигнализирует, что последовательность еще не выполнена до конца
		}
	}
	
*/

/*
	Обертка:
	
	Структура SequenceWrapper - это "обертка" для подмешиваемой последовательности, она хранит: 
	строковое имя функции; 
	адрес функции; 
	адрес класса, чьим методом является эта функция; 
	указатель на флаг завершенности функции (использующийся для сигнализации о выполнении последовательности). Данный буль-флаг инвертируется по завершении последовательности.

	Адрес класса-владельца нужно передавать в посдедовательность при вызове для того, чтобы она могла иметь доступ к членам этого класса, поскольку сама по себе является статической
*/

struct biCycleSequenceWrapper
{
	biCycleSequenceWrapper(std::string funcName, biCycleMixedSequenceFunc funcItself, void* funcOwnerClass, bool* bReportPtr = nullptr) 
	{
		eventName 	= funcName;
		bindedEvent = funcItself;
		bindedClass = funcOwnerClass;
		if (bReportPtr != nullptr) { report = bReportPtr; } //bReportPtr по умолчанию может быть nullptr (для необязательности его упоминания в аргументах), потому добавляем проверку
	}

	std::string eventName 	= "func_name";
	biCycleMixedSequenceFunc 	bindedEvent;
	void* 		bindedClass = nullptr;
	bool* 		report 		= nullptr; //указатель на булев флаг завершенности действия (для отчета, не обязателен)
};


class biCycleSequencer
{
	std::vector <biCycleSequenceWrapper> sequences;	//вектор с оболочками функций

public:
	// эта функция лишь принимает оберточную структуру функции и сохраняет ее в приватном векторе
	void CallSequence(biCycleSequenceWrapper newSequence)
	{
		sequences.push_back(newSequence);	}

	// функция Execute() должна вызываться из основной части программы. Например, можно вызывать ее в каждом цикле главной петли
	void Execute()
	{
		int i = 0;
		for (auto sequence : sequences)
		{
			bool result = sequences[i].bindedEvent(sequences[i].bindedClass); //для выбранной обертки последовательности sequences[i] вызываем исполнение хранимой в ней функции bindedEvent и передаем ей в качестве аргумента указатель sequences[i].bindedClass на класс-владелец метода последовательности 

			if (result) //если конкретная последовательность выполнена успешно, то отвязываем ее из потока
			{
				if (sequences[i].report != nullptr) { *sequences[i].report = !(*sequences[i].report); } //report из обертки функции иногда может быть nullptr, потому добавляем проверку и инвертируем присланный "извне" булев флаг.
				sequences.erase(sequences.cbegin() + i);
			}
			i++;
		}
	}

	~biCycleSequencer()
	{
		sequences.clear(); //очистка вектора последовательностей
	}
};
