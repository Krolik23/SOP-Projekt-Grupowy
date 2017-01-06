#pragma once
#include <iostream>
#include <string>
#include <list>
#include <vector>


using namespace std;

class PCB
{
public:
	enum class stanProcesu { Nowy, Czekaj¹cy, Gotowy, Aktywny, Zakoñczony, Zombie };

	stanProcesu Stan;
	int PID;
	int base;
	int limit;
	int idRodzica;
	vector<int> idPotomnych;
	/*Rejestry staloprzecinkowe(3)*/
	int A = 0;
	int B = 0; 
	int C=0;
	int licznikRozkazow = 0;
	string fileName;

	int timer;
	int tau;
	int tt;
	//-----------------------
	PCB();
	~PCB();

	PCB* fork(string p);
	PCB* fork();
	void wait(int ID);
	bool exit();

};
PCB* init(); //FUNKCJA URUCHAMIANA PRZY STARCIE SYSYEMU, TWORZY PIERWSZY PROCES "INIT"
void zmienStan(int PID, PCB::stanProcesu stan);
PCB* getProcess(int PID);
void drzewoProcesow();
void wypiszListe(list<PCB*> lista);
bool czyDziecko(int PID, int czyRodzicID);
bool kill(int ID);
void wpisz(PCB* a);

/*LISTY PROCESOW*/
struct Oczekiwanie	//istnienie struktury wytlumaczone ponizej
{
	int kto, naKogo;
};

extern list<PCB*> wszystkieProcesy;
extern list<PCB*> oczekujaceProcesy;
extern vector<Oczekiwanie> poWait;//gdyz musi zawierac rowniez informacje na ktory proces czeka
extern vector<int> poExit;
//extern list<PCB*> gotoweProcesy;

