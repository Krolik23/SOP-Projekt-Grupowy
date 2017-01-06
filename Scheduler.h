
#pragma once
#include <iostream>
#include <ctime>
#include <string>
#include "Procesy.h"
#include <cmath>


using namespace std;

class Scheduler {
public:
	static PCB* temp2;
	static PCB* temp3;
	static int t;
	static int thet1;
	static int tempthet;
	static int shortest;
	static int *theta;
	static list<PCB*> ReadyProcessList;
	static list<PCB*>::iterator it;
	static PCB *Running;
	static PCB *temp;
	static PCB *idle;
	static float alpha;
	static bool lock;
	static void Init() {
		it = ReadyProcessList.begin();
		Running = nullptr;
		t = 0;
		thet1 = 10;
		tempthet = 1;
		theta = new int[100];
		alpha = 0.5;
	}
	//Standardowo jestem wywo�ywany na t� metod� + jak przychodzi nowy proces to kto� wywo�uje mnie �ebym w og�le wiedzia� 
	static void Schedule() {
		// Ustawiam zmienn� t na czas jaki zaj�o wykonanie ostatniego procesu Running, ale tylko je�li nie wskazuje na nullptra
		if (Running != idle && Running != nullptr && Running->Stan == PCB::stanProcesu::Zako�czony) {
			t = Running->timer;
		}

		//sprawdzam, czy w systemie nie ma przypadkiem ju� aktywnego procesu, je�li jest nie podejmuj� �adnych dzia�a�
		if (Running != nullptr && Running != idle && Running->Stan == PCB::stanProcesu::Aktywny) {
			return;
		}

		// je�li nie ma...
		else {
			//...upewniam si�, �e lista gotowych proces�w na pewno nie jest pusta
			if (!ReadyProcessList.empty()) {
				int iterator = 1;
				//obliczam pierwsza thete liczac ja z gory ustalonej, wyimaginowanej, koncowo ustawiam wartosc thet1 na wartosc thety wybranego, najkrotszego procesu
				//tempthet = theta[0] = shortest = int((alpha * t) + (1 - alpha) * thet1);
				//ustawiam iterator na drugi proces na liscie, pierwszy juz obliczylem
				list<PCB*>::iterator i = ReadyProcessList.begin();
				list<PCB*>::iterator it = wszystkieProcesy.begin();
				// tymczasowo wskazuj� na pierwszy obliczony proces
				temp = ReadyProcessList.front();
				if (temp->tau == 0)
					shortest = 99999;
				else {
					shortest = temp->tau;
					cout << "Przewidywany czas wykonania procesu " << temp->PID << " to " << temp->tau << endl;
					advance(i, 1);
				}
				for (i; i != ReadyProcessList.end(); ++i) {
					temp2 = *i;
					if (temp2->Stan == PCB::stanProcesu::Gotowy) {
						if (temp2->tau == 0) {
							for (; it != wszystkieProcesy.end(); it++)
							{
								temp3 = *it;
								if (temp3->PID == temp2->PID - 1) {
									break;
								}
								/*theta[iterator] = int((alpha * t) + (1 - alpha) * theta[iterator - 1]);
								if (theta[iterator] < shortest) {
								shortest = theta[iterator];
								//Je�eli przewidywana praca jest mniejsza od ostatniej, to zn�w chc� ustawi� pole tymczasowe na proces, kt�ry wskazuje iterator listy gotowych proces�w
								temp = *i;
								tempthet = theta[iterator];*/
							}
							if (t != 0) {
								if (lock == false) {
									temp2->tau = int((alpha*t) + (1 - alpha)*Running->tau);
									if (temp2->tau <= 0) {
										temp2->tau = 1;
									}
									lock = true;
								}
								else
									temp2->tau = int((alpha*t) + (1 - alpha)*temp3->tau);
								if (temp2->tau <= 0) {
									temp2->tau = 1;
								}
							}
							else {
								temp2->tau = int((alpha*temp3->tau) + (1 - alpha)*temp3->tau);
							}
						}
						if (temp2->tau < shortest) {
							temp = temp2;
							shortest = temp2->tau;
						}
						cout << "Przewidywany czas wykonania procesu " << temp2->PID << " to " << temp2->tau << endl;
					}
				}
				Running = temp;
				//Zmieniam stan procesu, na kt�ry wskazuje Running na Aktywny
				//Running->zmienStan(Running->PID, PCB::stanProcesu::Aktywny);
				if (Running->tt != 0) {
					Running->tau = Running->tt;
					if (Running->tau <= 0)Running->tau = 1;
				}
			    zmienStan(Running->PID, PCB::stanProcesu::Aktywny);
				cout << "Wybieram proces: " << Running->PID << ". \n";
				lock = false;

			}
			//je�li jest pusta, ustawiam wska�nik na idle
			else {
				cout << "Brak gotowych proces�w na li�cie - IDLE. \n";
				Running = idle;
			}
		}
	}
private:
	Scheduler() {

	}

};
