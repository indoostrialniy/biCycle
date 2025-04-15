/*
	2025-04-15  indoostrialniy  <pawel.iv.2016@yandex.ru>
 
 	Пример использования библиотеки biCycle v1.5
 
	Функция main() содержит цикл while, имитирующий основной цикл.
	
	Сторожевой таймер watchDog прерывает выполнение основного цикла на 100-й итерации.
	
	Класс Body содержит метод последовательности bool testFunc(), которую можно подмешать в основной цикл. 
		Эта последовательность вызывает однократно колбек начала, отсчитывает свои собственные 30 раз выполнения с помощью внутреннего таймера timer и 
		ВОЗВРАЩАЕТ true, позволяя тем самым Секвенсору прекратить её выполнение и вызвать колбек конца.

	В теле функции main() по указателю body создается новый экземпляр класса Body, 
	создается и инициализируется сторожевой таймер watchDog и вспомогательная переменная mixStep для указания момента,
	когда можно	подмешать последовательность в основной цикл.
	
	Программа выводит в консоль соответствующие сообщения для объяснения логики работы.
	В данном примере подмешивание одной и той же последовательности testFunc() происходит единожды.
*/

#include "biCycle.h"


biCycle::Sequencer<bool> Flow;					// создали экземпляр без заданного типа данных



class Body										// класс-пример
{
public:
	bool testFunc()	// эта функция может вызываться Секвенсором в каждом цикле, будучи однажды подмешанной в него. По-умолчанию var = 0 !
	{		
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
		//return true;
	}
	
	int timer = 0;
	
	bool custom_start()
	{
		std::cout << "\t\t\tCustom start.\n";
		return true;
	}
	
	bool custom_end()
	{
		std::cout << "\t\t\tCustom end.\n";
		return true;
	}
	
	
	void load()
	{
		// 1.
		biCycle::Wrapper<bool> sequenceParameters;
			sequenceParameters.function = std::bind( &Body::testFunc, this);
			sequenceParameters.start_callback = std::bind( &Body::custom_start, this);	// по желанию
			sequenceParameters.end_callback = std::bind( &Body::custom_end, this);	// по желанию
			
		// 2. занесем сконфигурированную последовательность в секвенсор
		Flow.CallSequence( std::move(sequenceParameters) );										
	}
	
};






int main()
{
	std::cout << "Start of example of using library biCycle v1.4\n\n";
	
	Body* body = new Body;		// создаем новый экземпляр класса Body
	
	int watchDog = 0;			// счетчик сторожевого таймера
	
	int mixStep = 5;			// вспомогательная переменная для данного примера для указания, в какой момент времени подмешать функцию в поток
		
	while(1)
	{
		std::cout << "Main stream cycle: " << watchDog;
		
		// последовательность можно подмешать в основной цикл в любой момент. Для примера, сделаем это сначала на 5 итерации
		if(watchDog == mixStep)	
		{
			std::cout << "\nSend sequence testFunc() to execution at cycle: " << mixStep << "\n";
	
			body->load();
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
