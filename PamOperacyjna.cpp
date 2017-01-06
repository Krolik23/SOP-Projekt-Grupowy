#include "PamOperacyjna.h"
#include"Semaphore.h"
#include "Scheduler.h"
#include <fstream>
#include<string>

extern bool console;

//DONE
PamOperacyjna::PamOperacyjna()
{

	FSB *temp = new FSB;
	temp->pBloku = 0;
	temp->wBloku = 512;
	temp->next = NULL;
	head = temp;


}

char PamOperacyjna::RAM[512] = { 0 };

//TODO
PamOperacyjna::~PamOperacyjna()
{
	//usuniêcie listy FSB i pInMem
	//elementów FSB head pInMem head2
}

//DONE
void PamOperacyjna::setLimit(string nazwa_pliku, PCB* a)
{
	int rozmiar = 0;
	ifstream plik;
	plik.open(nazwa_pliku.c_str());
	string wiersz;

	while (!plik.eof()) {
		std::getline(plik, wiersz);
		//cout << wiersz << endl;
		rozmiar += wiersz.length();
		rozmiar++;				//odkomentuj je¿eli kompilator 
								//chce mieæ enter wpisany do tablicy jako znak
								//je¿eli to zrobisz to musisz te¿ odkomentowaæ w funkcji
								//memWrite(string nazwa_pliku)
	}

	plik.close();
	a->limit = rozmiar;
}

//DONE
void PamOperacyjna::FSBremove(PCB* a)
{
	FSB *temp = head;

	while (temp) {
		if (temp->pBloku == a->base) {
			temp->pBloku = a->base + a->limit;
			temp->wBloku -= a->limit;
			if (temp->wBloku);
		}

		temp = temp->next;
	}

}


//DONE /zak³ada ¿e base i limit s¹ ju¿ ustawione
void PamOperacyjna::FSBadd(PCB* a)
{
	FSB *nowy = new FSB;
	nowy->pBloku = a->base;
	nowy->wBloku = a->limit;
	FSB*temp = head;
	FSB*temp2 = head;
	FSB*poprzedni = temp2;
	temp2 = temp2->next;

	while (temp) {
		if (nowy->pBloku < head->pBloku) {
			nowy->next = temp;
			head = nowy;
			break;
		}
		else if (temp2 != NULL) {
			if (temp2->pBloku > nowy->pBloku   &&  poprzedni->pBloku < nowy->pBloku) {
				poprzedni->next = nowy;
				nowy->next = temp2;
				break;

			}
		}
		else if (temp->next == NULL) {
			temp->next = nowy;
			nowy->next = NULL;
			break;
		}
		temp = temp->next;
		poprzedni = poprzedni->next;
		temp2 = temp2->next;
	}
}


//DONE
void PamOperacyjna::FSBmerge()
{
	FSB *temp = head;
	FSB *poprzedni = head;

	while (temp) {

		if (temp->pBloku == poprzedni->pBloku + poprzedni->wBloku) {
			poprzedni->next = temp->next;
			poprzedni->wBloku += temp->wBloku;
			delete temp;
			temp = poprzedni->next;
		}
		else {
			poprzedni = temp;
			temp = temp->next;
		}
	}
}

//DONE
bool PamOperacyjna::isFreeFSB(PCB* a)
{
	int b;
	FSB *temp = head;
	while (temp) {
		if (a->limit > temp->wBloku) {
			b = 0;

		}
		else if (a->limit <= temp->wBloku) {
			b = 1;
			a->base = temp->pBloku;
			break;
		}
		temp = temp->next;
	}
	if (b == 1)return true;
	if (b == 0)return false;
}

//DONE
bool PamOperacyjna::isFreeMem(PCB* a)
{
	if (freeMem >= a->limit)
		return true;
	else return false;
}

//DONE
void PamOperacyjna::FreeMemUpdate()
{
	FSB*temp = head;
	freeMem = 0;
	while (temp) {
		freeMem += temp->wBloku;
		temp = temp->next;
	}

}

//DONE
void PamOperacyjna::XF(PCB *a)
{
	FreeMemUpdate();
	memClear(a);
	FSBadd(a);
	FSBmerge();
	removePfromList(a);
	a->base = -1;
	FreeMemUpdate();

	MEMORY.empty();
}


//DONE
bool PamOperacyjna::XA(string p, PCB *a)
{
	if (!(MEMORY.isOpen())) {
		if (console) {
			cout << "Brak miejsca";
			//MEMORY.P(Scheduler::Running);
			return false;
		}
		MEMORY.P(Scheduler::Running);
		return false;
	}
	FreeMemUpdate();
	if (a->limit == NULL) {
		setLimit(p, a);
	}
	if (isFreeFSB(a) != true) {
		if (isFreeMem(a) != true) {
			if (console) {
				cout << "Brak miejsca";
				//MEMORY.P(Scheduler::Running);
				return false;
			}
			MEMORY.P(Scheduler::Running);
			return false;
		}
		else if (isFreeMem(a) == true) {
			memMove();
			FSBmerge();
			isFreeFSB(a);
			FSBremove(a);
			addPtoList(a);
			memWrite(p, a);
		}
	}
	else if (isFreeFSB(a) == true) {
		memWrite(p, a);
		addPtoList(a);

		FSBremove(a);
	}

	FreeMemUpdate();
	FSB*temp = head;
	if (a->base == 0)temp->wBloku = temp->wBloku + 1;
	return true;
}

//DONE
void PamOperacyjna::memWrite(string nazwa_pliku, PCB *a)
{
	int komorka = a->base;
	fstream plik;
	string wiersz;
	//plik.open(nazwa_pliku.c_str());
	plik.open(nazwa_pliku);

	//gdzieœ tu powinno byæ wychwytywanie ¿e jak plik siê nie otworzy
	//to zwraca b³¹d, z tym ¿e nie wiem czy to jest potrzebne
	//jak ktoœ uzna ¿e tak to niech doda

	//a jak doda to ta pêtla while ni¿ej ma siê wykonaæ jak siê otwiera

	//DOPÓKI NIE DOJEDZIE DO KOÑCA PLIKU WYKONUJ PÊTLE
	while (!plik.eof()) {

		//ZCZYTANIE WIERSZA Z PLIKU
		std::getline(plik, wiersz);

		//SKOPIOWANIE WIERSZA DO POMOCNICZEJ TABLICY (STRING NA CHAR) 
		char strArray[150] = { 0 };
		strcpy_s(strArray, wiersz.c_str());

		//SKOPIOWANIE ZAWARTOSCI POMOCNICZEJ TABLICY DO KONKRETNEGO MIEJSCA W TABLICY RAM
		for (int i = 0; i < wiersz.length(); i++) {
			PamOperacyjna::RAM[komorka] = strArray[i];
			komorka++;
		}
		RAM[komorka] = '\n';			//odkomentuj jak interpreter chce mieæ
		komorka++;					//entery wpisane w tablice (znak entera \n)
									//je¿eli to zrobisz to musisz te¿ odkomentowaæ w funkcji
									//setLimit(string nazwa_pliku)

	}
	plik.close();
}
//DONE
void PamOperacyjna::memClear(PCB * a)
{
	for (int i = a->base; i < a->base + a->limit; i++)
		RAM[i] = '\0';
}

//DONE
void PamOperacyjna::memMove()
{

	int a = 0;
	int b = 0;


	if (head2 == NULL)
		cout << "nie ma nic w pamiêci";

	else {
		pInMem *temp = new pInMem;
		FSB *temp2 = head;
		int i = 0;
		while (i < size) {
			//SZUKA PIERWSZEGO KOLEJNEGO WOLNEGO MIEJSCA
			while (RAM[i] != '\0') {
				if (i >= size)break;
				i++;
			}
			//PRZYPISUJE NUMER ZNALEZIONEJ PUSTEJ KOMÓRKI ZMIENNEJ a
			if (i < size)
				a = i;
			//SZUKA PIERWSZE ZAJÊTE MIEJSCE
			while (RAM[i] == '\0') {
				if (i >= size)break;
				i++;
			}

			//JE¯ELI i NIE WYSZ£O POZA ZAKRES
			if (i < size) {
				//PRZYPISUJE NUMER PIERWSZEJ ZAJÊTEJ KOMÓRKI ZMIENNEJ b 
				b = i;
				//SZUKA PROCESU NA LIŒCIE PROCESÓW W PAMIÊCI, KTOREGO base=b
				temp = head2;
				while (temp != NULL) {
					if (temp->proc->base == b) {

						//ZMIENIA BASE ZNALEZIONEGO PROCESU NA WAROSC ZMIENNEJ a
						temp->proc->base = a;

						//tu dodaæ pbloku=temp->proc->base+temp->proc->limit
						while (temp2) {
							if (temp->proc->base != temp2->pBloku) {
								temp2 = temp2->next;
							}
							else temp2->pBloku = temp->proc->base + temp->proc->limit;
						}

						break;
					}
					else
						temp = temp->next;
				}
				//PRZEPISUJE DANE W WOLNE MIEJSCE I CZYŒCI KOMÓRKI GDZIE PROGRAM SIÊ ZNAJDOWA£
				if (a != b) {
					for (int j = 0; j < temp->proc->limit; j++) {
						RAM[a + j] = RAM[b + j];
						RAM[b + j] = '\0';
					}
				}
			}
		}
	}
}

void PamOperacyjna::showRAM()
{
	cout << "|";
	for (int i = 0; i < size; i++) {

		if (RAM[i] == '\0')
			cout << "-|";
		else if (RAM[i] == '\n')
			cout << " |";
		else
			cout << RAM[i] << "|";
		//if (i == 9)cout << endl;
		if (i>0 && (i + 1) % 20 == 0) {
			cout << "\n|";
		}

	}
}

//DONE
void PamOperacyjna::addPtoList(PCB *a)
{

	pInMem *nowy = new pInMem;
	nowy->proc = a;
	if (head2 != NULL) {
		pInMem *temp = head2;
		while (temp) {
			if (temp->next == NULL) {
				temp->next = nowy;
				nowy->next = NULL;
			}
			temp = temp->next;
		}
	}
	else {
		head2 = nowy;
		nowy->next = NULL;
	}

}

//DONE
void PamOperacyjna::removePfromList(PCB *a)
{
	pInMem *temp = head2;
	pInMem *poprzedni = head2;

	

	if (head2!=nullptr) {


		if (head2->proc == a) {
			head2 = head2->next;
			delete temp;
		}

	else {
		while (temp != NULL) {

			if (temp->proc == a) {
				poprzedni->next = temp->next;
				delete temp;
				temp = poprzedni->next;
			}
			else {
				poprzedni = temp;
				temp = temp->next;
			}
		}
	}

	}
}

//DONE
void PamOperacyjna::showPList()
{
	pInMem * glowa = head2;

	if (glowa == NULL)cout << "nie ma listy";
	else {
		cout << "lista procesow rezydujacych w pamieci\n";
		while (glowa) {
			cout << "ID: " << glowa->proc->PID;
			cout << " base: " << glowa->proc->base;
			cout << " limit: " << glowa->proc->limit << endl;
			glowa = glowa->next;
		}
	}
}
//DONE
void PamOperacyjna::showFSB()
{

	FSB* glowa = head;
	int numer = 0;
	if (glowa == NULL)cout << "nie ma listy";
	else {
		cout << endl;
		while (glowa) {
			numer++;
			cout << numer << ". blok na liscie FSB.\tpoczatek: " << glowa->pBloku;
			cout << "\twielkosc: " << glowa->wBloku << endl;
			glowa = glowa->next;
		}
	}
}

