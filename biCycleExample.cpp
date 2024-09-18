/*
	2024-09-18  indoostrialniy  <pawel.iv.2016@yandex.ru>
 
 	Пример использования библиотеки biCycle v1.0
 
	Функция main() содержит цикл while, имитирующий основной цикл.
	
	Сторожевой таймер watchDog прерывает выполнение основного цикла на 100-й итерации.
	
	Класс Body содержит метод последовательности static bool testFunc(void* ptr), которую можно подмешать в основной цикл. 
		Эта последовательность отсчитывает свои собственные 30 раз выполнения с помощью внутреннего таймера timer и 
		ВОЗВРАЩАЕТ true, позволяя тем самым Секвенсору прекратить её выполнение, А ТАКЖЕ выставляет true в разыменованный присланный "извне" указатель bool* bReportPtr для отчета.
		Явно прописывать bool* bReportPtr не обязательно, по-умолчанию он всегда будет nullptr.

	В теле функции main() по указателю body создается новый экземпляр класса Body, 
	создается и инициализируется как false булева переменная completeFlag для отслеживания статуса завершенности подмешиваемой последовательности,
	создается и инициализируется сторожевой таймер watchDog и вспомогательная переменная mixStep для указания момента,
	когда можно	подмешать последовательность в основной цикл.
	
	Программа выводит в консоль соответствующие сообщения для объяснения логики работы.
	В данном примере подмешивание одной и той же последовательности testFunc(void* ptr) происходит дважды - на 5-м и 50-м цикле.
*/

#include "biCycle.h"
biCycleSequencer Flow;	


class Body
{
	int timer = 0;

public:
	static bool testFunc(void* ptr)	// эта функция будет вызываться Секвенсором в каждом цикле
	{
		// для удобочитаемости введем define для принудительного восприятия "пустотного" указателя ptr как указателя на объект класса Body
		#define BODY ((Body*)ptr)
		
		if(BODY->timer == 30)		// условие завершения последовательности
		{
			std::cout << "\n\t\t\tMixed func has complete IT`S OWN INDEPENDENT(!) 30 cycles and set flag to True\n";
			BODY->timer = 0; // обнуляем таймер для обеспечения возможности повторного запуска последовательности
			return true;	}
		else 						// последовательность еще выполняется
		{
			std::cout << "\n\t\t\tMixed stream cycle: " << BODY->timer;
			BODY->timer++;
			return false;	}
	}
};




int main()
{
	std::cout << "Start of example\n";
	
	Body* body = new Body;		// создаем новый экземпляр класса Body
	
	bool completeFlag = false;	// переменная для хранения сигнала о завершении выполнения последовательности
	
	int watchDog = 0;			// счетчик сторожевого таймера
	
	int mixStep = 5;			// вспомогательная переменная для указания, в какой момент времени подмешать функцию в поток
	
	while(1)
	{
		std::cout << "Main stream cycle: " << watchDog;
		
		if(watchDog == mixStep)	// последовательность можно подмешать в основной цикл в любой момент
		{
			std::cout << "\nSend sequence testFunc(void* ptr) to execution at cycle: " << mixStep << "\n";
			Flow.CallSequence( {"testFunc", body->testFunc, body, &completeFlag} );								
			// либо
			//biCycleSequenceWrapper newSequence("testFunc", body->testFunc, body, &completeFlag);
			//Flow.CallSequence( newSequence );	
		}
		
		if(completeFlag)	// когда отчетный буль стал true
		{
			std::cout << "\nMain stream catched that mixed func returns TRUE!\n";
			mixStep = 50;
			completeFlag = false;		}
		
		if(watchDog == 100)
		{
			std::cout << "\nWatchdog timer has been triggered! Break!\n";
			break;			}
		
		Flow.Execute();
		
		watchDog++;
		std::cout << std::endl;
	}
	
	std::cout << "End of example\n";
	
	delete body;
	
	return 0;
}
