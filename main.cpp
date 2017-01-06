#include <cstdio>
#include "Interpreter.h"
#include "Scheduler.h"
#include "Procesy.h"
#include <string>
#include "SP_FAT8.h"
#include "Semaphore.h"

bool Scheduler::lock = false;
int Scheduler::t = 0;
int Scheduler::thet1 = 10;
int Scheduler::tempthet = 1;
int Scheduler::shortest = 0;
list<PCB*> Scheduler::ReadyProcessList;
list<PCB*>::iterator Scheduler::it;
float Scheduler::alpha = 0.5;
int* Scheduler::theta = nullptr;
PCB* Scheduler::Running = nullptr;
PCB* Scheduler::temp = nullptr;
PCB* Scheduler::temp2 = nullptr;
PCB* Scheduler::temp3 = nullptr;
PCB* Scheduler::idle = nullptr;

//Semaphores declarations
Semaphore MEMORY;
Semaphore semaphorePOST;

bool console;

int main()
{
	DYSK dysk;

	Interpreter interpreter = Interpreter();
	Scheduler::Init();
	Scheduler::idle = init(); 
	Scheduler::idle->tau = 999999;
	Scheduler::Schedule();  

	console = false;

	interpreter.boot();
	while (true) {
		cout << endl << "Wpisz instrukcje do wykonania lub:"<<endl<<"'c' aby wykonac nastepny rozkaz z pamieci"<<endl<<"'end' aby wyjsc"<< endl;
		string instrukcja;
		getline(cin, instrukcja);
		if (instrukcja == "c") { //takt procesora
			if (Scheduler::Running == nullptr || Scheduler::Running == Scheduler::idle) {
				Scheduler::Schedule();
			}
			if (Scheduler::Running == nullptr || Scheduler::Running == Scheduler::idle) //jesli proces wci¹¿ jest idle/nullptr to znaczy, ¿e nic nie ma
			{
				cout << "W kolejce nie ma gotowego procesu." << endl;
				//break;
			}
			else {
				console = false;
				interpreter.WykonajProgram(dysk, instrukcja);
			}
		}
		if (instrukcja != "c" &&instrukcja !="end") {
			console = true;
			interpreter.WykonajProgram(dysk, instrukcja);
		}
		if (instrukcja == "end") {
			break;
		}
		
	}

	return 0;
}