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
	//Standardowo jestem wywo�ywany na t� metod� + jak przychodzi nowy proces to kto� wywo�uje mnie �ebym w og�le wiedzia� 
	static void Schedule() {
		// Ustawiam zmienn� t na czas jaki zaj�o wykonanie ostatniego procesu Running, ale tylko je�li nie wskazuje na nullptra
		if (Running != nullptr) {
			t = Running->timer;
		}
		//sprawdzam, czy w systemie nie ma przypadkiem ju� aktywnego procesu, je�li jest nie podejmuj� �adnych dzia�a�
		if (Running != nullptr && Running->Stan == PCB::stanProcesu::Aktywny) {
			return;
		}
		// je�li nie ma...
		else {
			//...upewniam si�, �e lista gotowych proces�w na pewno nie jest pusta
			if (!ReadyProcessList.empty()) {
				int iterator = 1;
				//obliczam pierwsza thete liczac ja z gory ustalonej, wyimaginowanej, koncowo ustawiam wartosc thet1 na wartosc thety wybranego, najkrotszego procesu
				theta[0] = shortest = int((alpha * t) + (1 - alpha) * thet1);
				//ustawiam iterator na drugi proces na liscie, pierwszy juz obliczylem
				list<PCB*>::iterator i = ReadyProcessList.begin();
				// tymczasowo wskazuj� na pierwszy obliczony proces
				temp = ReadyProcessList.front();
				advance(i, 1);
				for (i; i != ReadyProcessList.end(); i++) {
					theta[iterator] = int((alpha * t) + (1 - alpha) * theta[iterator - 1]);
					if (theta[iterator] < shortest) {
						shortest = theta[iterator];
						//Je�eli przewidywana praca jest mniejsza od ostatniej, to zn�w chc� ustawi� pole tymczasowe na proces, kt�ry wskazuje iterator listy gotowych proces�w
						temp = *i;
					}
				}
				Running = temp;
				//Zmieniam stan procesu, na kt�ry wskazuje Running na Aktywny
				Running->zmienStan(Running->PID, PCB::stanProcesu::Aktywny);
			}
			//je�li jest pusta, ustawiam wska�nik na nullptr symbolizuj�cy bezczynno��
			else {
				Running = nullptr;
			}
		}
	}
private:
	Scheduler() {

	}
	
};