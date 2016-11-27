#pragma once
#include <iostream>
#include <ctime>
#include <string>
#include "PCB.h"
#include <cmath>
#include "Interpreter.h"

using namespace std;

class Scheduler {
public:
	static int t;
	static int thet1;
	static int shortest;
	static int *theta;
	static list<PCB*> ReadyProcessList;
	static PCB *Running;
	static PCB *temp;
	static float alpha;
	static void Init() {
		Running = nullptr;
		t = 0;
		thet1 = 10;
		theta = new int[100];
		alpha = 0.5;
	}
	//Standardowo jestem wywo³ywany na tê metodê + jak przychodzi nowy proces to ktoœ wywo³uje mnie ¿ebym w ogóle wiedzia³ 
	static void Schedule() {
		// Ustawiam zmienn¹ t na czas jaki zajê³o wykonanie ostatniego procesu Running, ale tylko jeœli nie wskazuje na nullptra
		if (Running != nullptr) {
			t = Running->timer;
		}
		//sprawdzam, czy w systemie nie ma przypadkiem ju¿ aktywnego procesu, jeœli jest nie podejmujê ¿adnych dzia³añ
		if (Running != nullptr && Running->Stan == PCB::stanProcesu::Aktywny) {
			return;
		}
		// jeœli nie ma...
		else {
			//...upewniam siê, ¿e lista gotowych procesów na pewno nie jest pusta
			if (!ReadyProcessList.empty()) {
				int iterator = 1;
				//obliczam pierwsza thete liczac ja z gory ustalonej, wyimaginowanej, koncowo ustawiam wartosc thet1 na wartosc thety wybranego, najkrotszego procesu
				theta[0] = shortest = int((alpha * t) + (1 - alpha) * thet1);
				//ustawiam iterator na drugi proces na liscie, pierwszy juz obliczylem
				list<PCB*>::iterator i = ReadyProcessList.begin();
				// tymczasowo wskazujê na pierwszy obliczony proces
				temp = ReadyProcessList.front();
				advance(i, 1);
				for (i; i != ReadyProcessList.end(); i++) {
					theta[iterator] = int((alpha * t) + (1 - alpha) * theta[iterator - 1]);
					if (theta[iterator] < shortest) {
						shortest = theta[iterator];
						//Je¿eli przewidywana praca jest mniejsza od ostatniej, to znów chcê ustawiæ pole tymczasowe na proces, który wskazuje iterator listy gotowych procesów
						temp = *i;
					}
				}
				Running = temp;
				//Zmieniam stan procesu, na który wskazuje Running na Aktywny
				Running->zmienStan(Running->PID, PCB::stanProcesu::Aktywny);
			}
			//jeœli jest pusta, ustawiam wskaŸnik na nullptr symbolizuj¹cy bezczynnoœæ
			else {
				Running = nullptr;
			}
		}
	}
private:
	Scheduler() {

	}
	
};