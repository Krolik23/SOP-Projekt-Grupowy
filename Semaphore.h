#pragma once
#include "Procesy.h"
#include <list>
class Semaphore
{
public:
	Semaphore() { open = true; }
	~Semaphore() {};

	std::list<PCB*> waiting;

	void opening() { if (!open) open = true; }

	void closing() { if (open) open = false; }

	//opuszczanie semafora
	void P(PCB *a) { //
		closing();
		zmienStan(a->PID, PCB::stanProcesu::Czekaj¹cy);  // zmiana stanu na oczekuj¹cy
		a->tt = a->tau;
		a->tau -= a->timer;
		waiting.push_back(a);
	}

	//podnoszenie semafora
	void V() {
			if (waiting.empty()) { opening(); }
			else {
				zmienStan(waiting.front()->PID, PCB::stanProcesu::Gotowy);
				waiting.pop_front();
				if (waiting.empty()) { opening(); }
			}
	}

	void tryPassing(PCB *a) {
		if (open)  zmienStan(a->PID, PCB::stanProcesu::Gotowy);		// zmiana stanu na gotowy
		else P(a);
	}

	void empty() {
		PCB* temp;
		for (int i = 0; i < waiting.size(); i++) {
			temp = waiting.front();
			V();
		}
	}

	bool isOpen() { return open; }

	void wyswietlListe() {
		wypiszListe(waiting);
	}
private:
	bool open;
};

extern Semaphore MEMORY;
extern Semaphore semaphorePOST;