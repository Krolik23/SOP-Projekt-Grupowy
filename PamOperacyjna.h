#pragma once
#include<iostream>
#include"Procesy.h"

using namespace std;
class PamOperacyjna
{
public:
	//Free Storage Block (element listy wolnych bloków pamiêci)
	struct FSB {
		int pBloku;
		int wBloku;
		FSB *next;
	};

	//proccess in memory (element listy procesów znajduj¹cych siê w pamiêci
	struct pInMem {
		PCB *proc;
		pInMem *next;
	};

	const int size = 512;				//zmienna okreœlaj¹ca wielkoœæ tablicy, z jakiegoœ powodu nie mogê	
	static char RAM[512];			//<----u¿yæ jej tutaj, ale przydaje siê w innych czêœciach kodu
									//wiêc jak trzeba zmieniæ rozmiar tablicy na potrzebê projektu to w tych 2 miejscach
	int freeMem = size;

	//destruktor/ konstruktor
	PamOperacyjna();
	~PamOperacyjna();
	FSB *head = new FSB;
	pInMem *head2 = NULL;


	//sprawdzanie wielkosci pliku-->ustawienie limit
	void setLimit(string nazwa_pliku, PCB* a);

	//zajêcie pamiêci-->zmniejszenie bloku na liœcie
	void FSBremove(PCB* a);

	//zwolnienie pamiêci-->dodanie do listy wolnego bloku pamiêci
	void FSBadd(PCB* a);

	//sprawdzanie czy bloki nie s¹ obok siebie a jak s¹ to je z³¹czyæ
	void FSBmerge();

	//czy jest wolny blok pamiêci
	bool isFreeFSB(PCB *a);

	//czy ca³kowita pamiêæ wolna jest wystarczaj¹ca
	bool isFreeMem(PCB* a);

	void FreeMemUpdate();

	//jak nie ma pamiêci to wys³aæ coœ tam na semafor ¿e zajête
	//void coœtam();

	void XF(PCB *a);//zwolnienie bloku pamiêci (przy usuwaniu procesu)
	bool XA(string nazwa_pliku, PCB *a);//przydzia³ pamiêci

										//wpisanie do pamieci
	void memWrite(string nazwa_pliku, PCB *a);

	//czyœci pamiêæ po procesie
	void memClear(PCB *a);

	//defragmentacja
	void memMove();
	//void XB();//cokolwiek to jest

	//void FSBupdate();

	void showRAM();

	//dodawanie procesu do listy procesów w pamiêci
	void addPtoList(PCB *a);


	//usuwanie procesu z listy procesów w pamieci
	void removePfromList(PCB*a);

	void showPList();
	void showFSB();


};