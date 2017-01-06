#pragma once
#include<iostream>
#include"Procesy.h"

using namespace std;
class PamOperacyjna
{
public:
	//Free Storage Block (element listy wolnych blok�w pami�ci)
	struct FSB {
		int pBloku;
		int wBloku;
		FSB *next;
	};

	//proccess in memory (element listy proces�w znajduj�cych si� w pami�ci
	struct pInMem {
		PCB *proc;
		pInMem *next;
	};

	const int size = 512;				//zmienna okre�laj�ca wielko�� tablicy, z jakiego� powodu nie mog�	
	static char RAM[512];			//<----u�y� jej tutaj, ale przydaje si� w innych cz�ciach kodu
									//wi�c jak trzeba zmieni� rozmiar tablicy na potrzeb� projektu to w tych 2 miejscach
	int freeMem = size;

	//destruktor/ konstruktor
	PamOperacyjna();
	~PamOperacyjna();
	FSB *head = new FSB;
	pInMem *head2 = NULL;


	//sprawdzanie wielkosci pliku-->ustawienie limit
	void setLimit(string nazwa_pliku, PCB* a);

	//zaj�cie pami�ci-->zmniejszenie bloku na li�cie
	void FSBremove(PCB* a);

	//zwolnienie pami�ci-->dodanie do listy wolnego bloku pami�ci
	void FSBadd(PCB* a);

	//sprawdzanie czy bloki nie s� obok siebie a jak s� to je z��czy�
	void FSBmerge();

	//czy jest wolny blok pami�ci
	bool isFreeFSB(PCB *a);

	//czy ca�kowita pami�� wolna jest wystarczaj�ca
	bool isFreeMem(PCB* a);

	void FreeMemUpdate();

	//jak nie ma pami�ci to wys�a� co� tam na semafor �e zaj�te
	//void co�tam();

	void XF(PCB *a);//zwolnienie bloku pami�ci (przy usuwaniu procesu)
	bool XA(string nazwa_pliku, PCB *a);//przydzia� pami�ci

										//wpisanie do pamieci
	void memWrite(string nazwa_pliku, PCB *a);

	//czy�ci pami�� po procesie
	void memClear(PCB *a);

	//defragmentacja
	void memMove();
	//void XB();//cokolwiek to jest

	//void FSBupdate();

	void showRAM();

	//dodawanie procesu do listy proces�w w pami�ci
	void addPtoList(PCB *a);


	//usuwanie procesu z listy proces�w w pamieci
	void removePfromList(PCB*a);

	void showPList();
	void showFSB();


};