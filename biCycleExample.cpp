/*
	2024-10-15  indoostrialniy  <pawel.iv.2016@yandex.ru>
 
 	Пример использования библиотеки biCycle v1.3
 
	Функция main() содержит цикл while, имитирующий основной цикл.
	
	Сторожевой таймер watchDog прерывает выполнение основного цикла на 100-й итерации.
	
	Класс Body содержит метод последовательности bool testFunc(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data), которую можно подмешать в основной цикл. 
		Эта последовательность вызывает однократно колбек начала, отсчитывает свои собственные 30 раз выполнения с помощью внутреннего таймера timer и 
		ВОЗВРАЩАЕТ true, позволяя тем самым Секвенсору прекратить её выполнение и вызвать колбек конца.

	В теле функции main() по указателю body создается новый экземпляр класса Body, 
	создается и инициализируется как false булева переменная completeFlag для отслеживания статуса завершенности подмешиваемой последовательности,
	создается и инициализируется сторожевой таймер watchDog и вспомогательная переменная mixStep для указания момента,
	когда можно	подмешать последовательность в основной цикл.
	
	Программа выводит в консоль соответствующие сообщения для объяснения логики работы.
	В данном примере подмешивание одной и той же последовательности testFunc(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data) происходит дважды - на 5-м и 50-м цикле.
*/

#include "biCycle.h"

struct myData	// собственная структура данных позволит при дальнейшем масштабировании не перелопачивать весь код, добавляя аргументы. Т.о - задел на будущее
{	
	myData( bool* bReportPtr = nullptr, int value = 0 ) : report(bReportPtr), var(value) {}
	
	bool* 	report 	= nullptr; 	//указатель на булев флаг завершенности действия (для отчета, не обязателен)
	int 	var 	= 0;		// вспомогательная переменная
};


#define BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE myData



biCycleSequencer<BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE> Flow;	// создали экземпляр с заданным типом данных



class Body										// класс-пример
{
	int timer = 0;								// приватный член класса - таймер для обеспечения отсчетов

public:
	bool testFunc(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)	// эта функция может вызываться Секвенсором в каждом цикле, будучи однажды подмешанной в него. По-умолчанию var = 0 !
	{
		// для удобочитаемости введем define для принудительного восприятия "пустотного" указателя ptr как указателя на объект класса Body
		//#define BODY ((Body*)ptr)
		
		// о способах более безопасного выполнения кода последовательности см. biCycle.h "Шаблон, которого должна придерживаться пользовательская последовательность (функция/метод)."
		
		
		if(timer == 30)		// условие завершения последовательности
		{
			std::cout << "\n\t\t\tMixed func has complete IT`S OWN INDEPENDENT(!) 30 cycles and set flag to True\n";
			timer = 0; // обнуляем таймер для обеспечения возможности повторного запуска последовательности
			return true;	
		}
		else 						// последовательность еще выполняется
		{
			std::cout << "\n\t\t\tMixed stream cycle: " << timer;
			//if(var != 0) {std::cout << "\tAlso have an aux int variable: " << var;}
			timer++;
			return false;	
		}
		
	}
	
	
	bool custom_start(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
	{
		std::cout << "\n\t\tCustom start. timer=50\n";
		timer += 10;
		return true;
	}
	
	bool custom_end(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE data)
	{
		std::cout << "\n\tYohohohoh, its ready!!\n";
		*(data.report) = !*(data.report);
		return true;
	}
	
	
	void load(bool& completeFlag, int auxVar = 0)
	{
		//~ Flow.CallSequence( { std::bind( &Body::testFunc, this), biCycleSequenceData(&completeFlag, auxVar) } );	
		
		BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE datas(&completeFlag, auxVar);
		std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> function = std::bind( &Body::testFunc, this, datas);
		std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> start = std::bind( &Body::custom_start, this, datas);
		std::function<bool(BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE)> end = std::bind( &Body::custom_end, this, datas);


		//~ Flow.CallSequence( { std::move(startFunction), std::move(datas) } );	
		
		Flow.CallSequence( { std::move(function), std::move(start), std::move(end), std::move(datas) } );	
	}
};






int main()
{
	std::cout << "Start of example of using library biCycle v1.3\n\n";
	
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
			
			// способ 1
			//~ std::function<bool(biCycleSequenceData)> startFunction = std::bind( &Body::testFunc, body);
			//~ biCycleSequenceData datas(&completeFlag, auxVar);
			//~ Flow.CallSequence( { std::move(startFunction), std::move(datas) } );								
			
			// способ 2
			//~ Flow.CallSequence( { std::bind( &Body::testFunc, body), biCycleSequenceData(&completeFlag, auxVar) } );	
			body->load(completeFlag, 0);
			// способ 3
			//~ Flow.CallSequence( { std::bind( &Body::testFunc, body), biCycleSequenceData() } );	
			
			// способ 4
			//~ Flow.CallSequence( { std::bind( &Body::testFunc, body) } );	

			
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
		std::cout << "\n";
	}
	
	std::cout << "End of example\n";
	
	delete body;
	
	return 0;
}
