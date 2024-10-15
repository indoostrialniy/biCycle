/*
	2024-10-15  indoostrialniy  <pawel.iv.2016@yandex.ru>
 
 	Пример использования библиотеки biCycle v1.2
 
	Функция main() содержит цикл while, имитирующий основной цикл.
	
	Сторожевой таймер watchDog прерывает выполнение основного цикла на 100-й итерации.
	
	Класс Body содержит метод последовательности static bool testFunc(void* ptr, int var), которую можно подмешать в основной цикл. 
		Эта последовательность отсчитывает свои собственные 30 раз выполнения с помощью внутреннего таймера timer и 
		ВОЗВРАЩАЕТ true, позволяя тем самым Секвенсору прекратить её выполнение, А ТАКЖЕ инвертировать разыменованный присланный "извне" указатель bool* bReportPtr для отчета.
		Явно прописывать bool* bReportPtr при подмешивании не обязательно, по-умолчанию он всегда будет nullptr.
		Явно прописывать int auxVar при подмешивании не обязательно, по-умолчанию она всегда будет 0.

	В теле функции main() по указателю body создается новый экземпляр класса Body, 
	создается и инициализируется как false булева переменная completeFlag для отслеживания статуса завершенности подмешиваемой последовательности,
	создается и инициализируется сторожевой таймер watchDog и вспомогательная переменная mixStep для указания момента,
	когда можно	подмешать последовательность в основной цикл.
	
	Программа выводит в консоль соответствующие сообщения для объяснения логики работы.
	В данном примере подмешивание одной и той же последовательности testFunc(void* ptr, int var) происходит дважды - на 5-м и 50-м цикле.
*/

#include "biCycle.h"
biCycleSequencer Flow;	


class Body										// класс-пример
{
	int timer = 0;								// приватный член класса - таймер для обеспечения отсчетов

public:
	static bool testFunc(void* ptr, int var)	// эта функция может вызываться Секвенсором в каждом цикле, будучи однажды подмешанной в него. По-умолчанию var = 0 !
	{
		// для удобочитаемости введем define для принудительного восприятия "пустотного" указателя ptr как указателя на объект класса Body
		#define BODY ((Body*)ptr)
		
		// о способах более безопасного выполнения кода последовательности см. biCycle.h "Шаблон, которого должна придерживаться пользовательская последовательность (функция/метод)."
		
		
		if(BODY->timer == 30)		// условие завершения последовательности
		{
			std::cout << "\n\t\t\tMixed func has complete IT`S OWN INDEPENDENT(!) 30 cycles and set flag to True\n";
			BODY->timer = 0; // обнуляем таймер для обеспечения возможности повторного запуска последовательности
			return true;	}
		else 						// последовательность еще выполняется
		{
			std::cout << "\n\t\t\tMixed stream cycle: " << BODY->timer;
			if(var != 0) {std::cout << "\tAlso have an aux int variable: " << var;}
			BODY->timer++;
			return false;	}
	}
};




int main()
{
	std::cout << "Start of example of using library biCycle v1.2\n";
	
	Body* body = new Body;		// создаем новый экземпляр класса Body
	
	bool completeFlag = false;	// переменная для хранения сигнала о завершении выполнения последовательности
	
	int watchDog = 0;			// счетчик сторожевого таймера
	
	int mixStep = 5;			// вспомогательная переменная для данного примера для указания, в какой момент времени подмешать функцию в поток
	
	int& auxVar = mixStep;		// ссылочная переменная-копия, которую передаем в подмешиваемую последовательность в качестве аргумента наряду с адресом экземпляра класса-владельца. Можно указать любое требуемое число
	
	while(1)
	{
		std::cout << "Main stream cycle: " << watchDog;
		
		// последовательность можно подмешать в основной цикл в любой момент. Для примера, сделаем это сначала на 5 итерации
		if(watchDog == mixStep)	
		{
			std::cout << "\nSend sequence testFunc(void* ptr, int var) to execution at cycle: " << mixStep << "\n";
			Flow.CallSequence( {"testFunc", body->testFunc, body, &completeFlag, auxVar} );								
			// либо, к примеру, обойтись без явного указания ссылочной переменной и закинуть функцию в Секвенсор через отдельно объявленную структуру-обертку
			//biCycleSequenceWrapper newSequence("testFunc", body->testFunc, body, &completeFlag);
			//Flow.CallSequence( newSequence );	
		}
		
		if(completeFlag)	// когда отчетный буль стал true
		{
			std::cout << "\nMain stream catched that mixed func returns TRUE! Set mixStep = 50.\n";
			mixStep = 50;
			completeFlag = false;	// так как при явном указании булева флага о статусе выполнения функции по ее завершении он ИНВЕРТИРУЕТСЯ, то для повторного запуска необходимо заново установить его в false!
		}
		
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
