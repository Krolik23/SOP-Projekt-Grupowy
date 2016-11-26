#pragma once
#include <iostream>
#include <string>
#include "PostOffice.h" //komunikacja
#include "Procesy.h" 
#include "Scheduler.h"  //procesor
#include "PamOperacyjna.h"
#include "SP_FAT8.h"     //pliki

using namespace std;

class Interpreter {

public:
	int PID;
	int rejestrA;
	int rejestrB;
	int rejestrC;
	int LicznikRozkazow;

	Interpreter() {
		PID = 0;
		rejestrA = 0;
		rejestrB = 0;
		rejestrC = 0;
		LicznikRozkazow = 0;
	}

	void PobierzRejestry() {
		PID = Running->PID;
		rejestrA = Running->A;
		rejestrB = Running->B;
		rejestrC = Running->C;
		LicznikRozkazow = Running->licznikRozkazow;
	}
	string PobierzRozkaz() {
		string rozkaz;
		for (int i = Running->ogrPam[1]; i < (Running->ogrPam[1] + Running->ogrPam[2]); i++) {
			rozkaz += RAM[i];
		}
		return rozkaz;
	}
	void StanRejestrow() {
		cout << "PID: " << PID << endl;
		cout << "A: " << rejestrA << endl;
		cout << "B: " << rejestrB << endl;
		cout << "C: " << rejestrC << endl;
		cout << "Licznik Rozkazow: " << LicznikRozkazow << endl;
	}
	void ZapiszRejestry() {
		Running->A = rejestrA;
		Running->B = rejestrB;
		Running->C = rejestrC;
		Running->licznikRozkazow = LicznikRozkazow;
	}

	void WykonajProgram() {
		string rozkaz = PobierzRozkaz();
		PobierzRejestry();
		string operacja = rozkaz.substr(0, 2);
		cout << endl << "Wykonywany rozkaz: " << operacja << endl;
		if (operacja == "AD") { //sumowanie rejestr A z B lub C
			string rejestr = rozkaz.substr(3, 1);
			if (rejestr == "B") rejestrA += rejestrB;
			if (rejestr == "C") rejestrA += rejestrC;
		}
		else if (operacja == "DC") { //zmniejsza rejestr o 1
			string rejestr = rozkaz.substr(3, 1);
			if (rejestr == "A") rejestrA -= 1;
			if (rejestr == "B") rejestrB -= 1;
			if (rejestr == "C") rejestrC -= 1;
		}
		else if (operacja == "SB") { //odejmowanie od A rejestru B lub C
			string rejestr = rozkaz.substr(3, 1);
			if (rejestr == "B") rejestrA -= rejestrB;
			if (rejestr == "C") rejestrA -= rejestrC;
		}
		else if (operacja == "ML") { //mnozenie rejestr A przez B lub C
			string rejestr = rozkaz.substr(3, 1);
			if (rejestr == "B") rejestrA *= rejestrB;
			if (rejestr == "C") rejestrA *= rejestrC;
		}
		else if (operacja == "IC") { //zwiêksza rejestr o 1
			string rejestr = rozkaz.substr(3, 1);
			if (rejestr == "A") rejestrA += 1;
			if (rejestr == "B") rejestrB += 1;
			if (rejestr == "C") rejestrC += 1;
		}
		else if (operacja == "DV") { //dzielenie rejestru B lub C przez A
			string rejestr = rozkaz.substr(3, 1);
			if (rejestr == "B") rejestrA = rejestrB / rejestrA;
			if (rejestr == "C") rejestrA = rejestrC / rejestrA;
		}
		else if (operacja == "MV") { //kopiuje wartosc do danego rejestru
			string rejestr = rozkaz.substr(3, 1);
			string wartosc = rozkaz.substr(5, 3);
			int liczba;
			if (wartosc[2] < 48 || wartosc[2]>57) {
				if (wartosc[1] < 48 || wartosc[1]>57) {
					liczba = (wartosc[0] - 48);
				}
				else liczba = (wartosc[0] - 48) * 10 + (wartosc[1] - 48);
			}
			else liczba = (wartosc[0] - 48) * 100 + (wartosc[1] - 48) * 10 + (wartosc[2] - 48);

			if (rejestr == "A") rejestrA = liczba;
			if (rejestr == "B") rejestrB = liczba;
			if (rejestr == "C") rejestrC = liczba;
		}
		else if (operacja == "JP") { //skok do innego rozkazu przez zmianê licznika
			string wartosc = rozkaz.substr(3, 2);
			int liczba;
			if (wartosc[1] < 48 || wartosc[1]>57) {
				liczba = (wartosc[0] - 48);
			}
			else liczba = (wartosc[0] - 48) * 10 + (wartosc[1] - 48);
			LicznikRozkazow = liczba;
		}
		else if (operacja == "CL") { //czyszczenie ekranu
			system("cls");
		}

		if (operacja == "SP") { //stworz plik
			DYSK dysk;
			string nazwa = rozkaz.substr(3, 3);
			string rozsz = rozkaz.substr(7, 3);
			dysk.create_file(nazwa, rozsz);
		}
		if (operacja == "ZP") { //zapisz do pliku
			DYSK dysk;
			string nazwa = rozkaz.substr(3, 3);
			string rozsz = rozkaz.substr(7, 3);
			string data = rozkaz.substr(11, 3);
			dysk.write_file(nazwa, rozsz, data);
		}
		if (operacja == "UP") { //usun plik
			DYSK dysk;
			string nazwa = rozkaz.substr(3, 3);
			string rozsz = rozkaz.substr(7, 3);
			dysk.delete_file(nazwa, rozsz);
		}
		if (operacja == "CP") { //czytanie z pliku
			DYSK dysk;
			string nazwa = rozkaz.substr(3, 3);
			string rozsz = rozkaz.substr(7, 3);
			dysk.print_file(nazwa, rozsz);
		}
		if (operacja == "LP") { //lista plikow
			DYSK dysk;
			dysk.view_files();
		}
		if (operacja == "NP") { //zmiana nazwy pliku
			DYSK dysk;
			string nazwa = rozkaz.substr(3, 3);
			string rozsz = rozkaz.substr(7, 3);
			string nowa_nazwa = rozkaz.substr(11, 3);
			dysk.rename_file(nazwa, rozsz, nowa_nazwa);
		}

		if (operacja == "DP") { //dopisac do pliku
			DYSK dysk;
			string nazwa = rozkaz.substr(3, 3);
			string rozsz = rozkaz.substr(7, 3);
			string dane = rozkaz.substr(11, 3);
			dysk.append_file(nazwa, rozsz, dane);
		}

		if (operacja == "XW") { //pokaz wszystkie procesy
			wypiszListe(list<PCB*>wszystkieProcesy);
		}

		if (operacja == "XG") { //pokaz gotowe procesy
			wypiszListe(list<PCB*>gotoweProcesy);
		}

		if (operacja == "XO") { //pokaz oczekujace procesy
			wypiszListe(list<PCB*>oczekujaceProcesy);
		}

		else if (operacja == "XC") { //utworzenie procesu
			string rodzic = rozkaz.substr(3, 2);
			int PID;

			if (rodzic[1] == 32) {
				PID = rodzic[0] - 48;
			}
			else {
				PID = (rodzic[0] - 48) * 10 + (rodzic[1] - 48);
			}

			PCB* parent = getProcess(PID);
			parent->fork();
		}
		else if (operacja == "XD") { //usuniecie procesu
			string wartosc = rozkaz.substr(3, 2);
			int PID;

			if (wartosc[1] == 32) {
				PID = wartosc[0] - 48;
			}
			else {
				PID = (wartosc[0] - 48) * 10 + (wartosc[1] - 48);
			}

			kill(PID);
		}
		else if (operacja == "XN") { //znalezienie procesu o PID
			string wartosc = rozkaz.substr(3, 2);
			int PID;

			if (wartosc[1] == 32) {
				PID = wartosc[0] - 48;
			}
			else {
				PID = (wartosc[0] - 48) * 10 + (wartosc[1] - 48);
			}
			if (getProcess(PID) == nullptr) {
				cout << endl << "Nie znaleziono procesu" << endl;
			}
			else {
				cout << endl << "Proces znaleziono" << endl;
			}
		}
		else if (operacja == "XR") { //czytanie komunikatu
			PostOffice skrzynka;
			cout << endl << "Komunikat: " << skrzynka.receive(PID) << endl;
		}
		else if (operacja == "XS") { //wysylanie komunikatu
			PostOffice skrzynka;
			string odbiorca = rozkaz.substr(3, 2);
			int recipientPID;
			string wartosc;

			if (odbiorca[1] == 32) {
				recipientPID = odbiorca[0] - 48;
				wartosc = rozkaz.substr(5, 3);
			}
			else {
				recipientPID = (odbiorca[0] - 48) * 10 + (odbiorca[1] - 48);
				wartosc = rozkaz.substr(6, 3);
			}

			string komunikat;
			if ((wartosc[2] >64 && wartosc[2]<91) || (wartosc[2]>96 && wartosc[2]<123)) {
				komunikat = wartosc.substr(0, 3);
				Message wiadomosc(PID, komunikat);
				skrzynka.send(&wiadomosc, recipientPID);
			}
			else {
				if ((wartosc[1] > 64 && wartosc[1] < 91) || (wartosc[1]>96 && wartosc[1] < 123)) {
					komunikat = wartosc.substr(0, 2);
					Message wiadomosc(PID, komunikat);
					skrzynka.send(&wiadomosc, recipientPID);
				}
				else {
					komunikat = wartosc.substr(0, 1);
					Message wiadomosc(PID, komunikat);
					skrzynka.send(&wiadomosc, recipientPID);
				}
			}
		}
		else
		{
			cout << endl << "Nie rozpoznano polecenia" << endl;
		}
		StanRejestrow();
		ZapiszRejestry();
		LicznikRozkazow = LicznikRozkazow + rozkaz.length();
	}

	string WczytajInstrukcje() {
		cout << "Podaj instrukcje" << endl;
		string instrukcja;
		getline(cin, instrukcja);
		return instrukcja;
	}
};