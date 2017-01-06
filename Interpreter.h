#pragma once
#include <iostream>
#include <string>
#include "PostOffice.h" //komunikacja
#include "Procesy.h"
#include "Scheduler.h"  //procesor
#include "PamOperacyjna.h"
#include "Semaphore.h"
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
		PID = Scheduler::Running->PID;
		rejestrA = Scheduler::Running->A;
		rejestrB = Scheduler::Running->B;
		rejestrC = Scheduler::Running->C;
		LicznikRozkazow = Scheduler::Running->licznikRozkazow;
	}
	string PobierzRozkaz() {
		if (Scheduler::Running == nullptr)   return "";

		string rozkaz;
		int i = Scheduler::Running->base + LicznikRozkazow;
		while (PamOperacyjna::RAM[i] != '\n'&&i <= Scheduler::Running->limit + Scheduler::Running->base) {
			if (i == Scheduler::Running->limit + Scheduler::Running->base) { break; }
			rozkaz += PamOperacyjna::RAM[i];
			i++;
		}
		return rozkaz;
	}
	void StanRejestrow() {
		cout << endl << "PID: " << PID << endl;
		cout << "A: " << rejestrA << endl;
		cout << "B: " << rejestrB << endl;
		cout << "C: " << rejestrC << endl;
		cout << "Licznik Rozkazow: " << LicznikRozkazow << endl;
		cout << "*********************************" << endl << endl << endl;
	}
	void ZapiszRejestry() {
		Scheduler::Running->A = rejestrA;
		Scheduler::Running->B = rejestrB;
		Scheduler::Running->C = rejestrC;
		Scheduler::Running->licznikRozkazow = LicznikRozkazow;
		Scheduler::Running->timer++;
		Scheduler::Running->tt++;
	}

	void WykonajProgram(DYSK &dysk, string instrukcja) {
		PobierzRejestry();
		string rozkaz;
		string operacja;
		int pomocnicza = 0;
		int pomocnicza2 = 0;
		if (instrukcja == "c") {
			rozkaz = PobierzRozkaz();
			operacja = rozkaz.substr(0, 2);
		}
		else {
			rozkaz = instrukcja;
			operacja = rozkaz.substr(0, 2);
		}
		cout << endl << endl << endl << "*********************************" << endl;
		cout << "Wykonywany rozkaz: " << rozkaz << endl;
		if (operacja == "EX") {
			StanRejestrow();
			LicznikRozkazow = LicznikRozkazow + rozkaz.length();
			ZapiszRejestry();
			Scheduler::Schedule();
			kill(Scheduler::Running->PID);
			Scheduler::Schedule();
		}
		else {
			if (operacja == "AD") { //sumowanie rejestr A z B lub C
				if (rozkaz.length() != 4 || rozkaz.substr(2, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string rejestr = rozkaz.substr(3, 1);
					if (rejestr == "B") {
						rejestrA += rejestrB;
					}
					else if (rejestr == "C") {
						rejestrA += rejestrC;
					}
					else {
						cout << endl << "Niepoprawna instrukcja!" << endl;
					}
				}
			}
			else if (operacja == "DC") { //zmniejsza rejestr o 1
				if (rozkaz.length() != 4 || rozkaz.substr(2, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string rejestr = rozkaz.substr(3, 1);
					if (rejestr == "A") {
						rejestrA -= 1;
					}
					else if (rejestr == "B") {
						rejestrB -= 1;
					}
					else if (rejestr == "C") {
						rejestrC -= 1;
					}
					else {
						cout << endl << "Niepoprawna instrukcja!" << endl;
					}
				}
			}
			else if (operacja == "SB") { //odejmowanie od A rejestru B lub C
				if (rozkaz.length() != 4 || rozkaz.substr(2, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string rejestr = rozkaz.substr(3, 1);
					if (rejestr == "B") {
						rejestrA -= rejestrB;
					}
					else if (rejestr == "C") {
						rejestrA -= rejestrC;
					}
					else {
						cout << endl << "Niepoprawna instrukcja!" << endl;
					}
				}
			}
			else if (operacja == "ML") { //mnozenie rejestr A przez B lub C
				if (rozkaz.length() != 4 || rozkaz.substr(2, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string rejestr = rozkaz.substr(3, 1);
					if (rejestr == "B") {
						rejestrA *= rejestrB;
					}
					else if (rejestr == "C") {
						rejestrA *= rejestrC;
					}
					else {
						cout << endl << "Niepoprawna instrukcja!" << endl;
					}
				}
			}
			else if (operacja == "IC") { //zwiêksza rejestr o 1
				if (rozkaz.length() != 4 || rozkaz.substr(2, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string rejestr = rozkaz.substr(3, 1);
					if (rejestr == "A") {
						rejestrA += 1;
					}
					else if (rejestr == "B") {
						rejestrB += 1;
					}
					else if (rejestr == "C") {
						rejestrC += 1;
					}
					else {
						cout << endl << "Niepoprawna instrukcja!" << endl;
					}
				}
			}
			else if (operacja == "DV") { //dzielenie rejestru B lub C przez A
				if (rozkaz.length() != 4 || rozkaz.substr(2, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string rejestr = rozkaz.substr(3, 1);
					if (rejestr == "B") {
						rejestrA = rejestrB / rejestrA;
					}
					else if (rejestr == "C") {
						rejestrA = rejestrC / rejestrA;
					}
					else {
						cout << endl << "Niepoprawna instrukcja!" << endl;
					}
				}
			}
			else if (operacja == "MV") { //kopiuje wartosc do danego rejestru
				if (rozkaz.length() < 6 || rozkaz.substr(2, 1) != " " || rozkaz.substr(4, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string rejestr = rozkaz.substr(3, 1);
					string wartosc = rozkaz.substr(5, rozkaz.length() - 5);
					int liczba;
					if (wartosc.length() == 3) {
						if ((wartosc[0] < 48 || wartosc[0]>57) && (wartosc[1] < 48 || wartosc[1]>57) && (wartosc[2] < 48 || wartosc[2]>57)) {
							cout << endl << "Niepoprawna instrukcja!" << endl;
						}
						else {
							liczba = (wartosc[0] - 48) * 100 + (wartosc[1] - 48) * 10 + (wartosc[2] - 48);
							if (rejestr == "A") {
								rejestrA = liczba;
							}
							else if (rejestr == "B") {
								rejestrB = liczba;
							}
							else if (rejestr == "C") {
								rejestrC = liczba;
							}
							else {
								cout << endl << "Niepoprawny rejestr!" << endl;
							}
						}
					}
					else if (wartosc.length() == 2) {
						if ((wartosc[0] < 48 || wartosc[0]>57) && (wartosc[1] < 48 || wartosc[1]>57)) {
							cout << endl << "Niepoprawna instrukcja!" << endl;
						}
						else {
							liczba = (wartosc[0] - 48) * 10 + (wartosc[1] - 48);
							if (rejestr == "A") {
								rejestrA = liczba;
							}
							else if (rejestr == "B") {
								rejestrB = liczba;
							}
							else if (rejestr == "C") {
								rejestrC = liczba;
							}
							else {
								cout << endl << "Niepoprawny rejestr!" << endl;
							}
						}
					}
					else if (wartosc.length() == 1) {
						if ((wartosc[0] < 48 || wartosc[0]>57)) {
							cout << endl << "Niepoprawna instrukcja!" << endl;
						}
						else {
							liczba = (wartosc[0] - 48);
							if (rejestr == "A") {
								rejestrA = liczba;
							}
							else if (rejestr == "B") {
								rejestrB = liczba;
							}
							else if (rejestr == "C") {
								rejestrC = liczba;
							}
							else {
								cout << endl << "Niepoprawny rejestr!" << endl;
							}
						}
					}
				}
			}
			else if (operacja == "JP") { //skok do innego rozkazu przez zmianê licznika
				if (rozkaz.length()<4 || rozkaz.substr(2, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					if (rozkaz.length() == 4) {
						string wartosc = rozkaz.substr(3, 1);
						int liczba;
						if (wartosc[0] < 48 || wartosc[0]>57) {
							liczba = (wartosc[0] - 48);
							LicznikRozkazow = liczba;
						}
						else {
							cout << endl << "Niepoprawna instrukcja!" << endl;
						}
					}
					if (rozkaz.length() == 5) {
						string wartosc = rozkaz.substr(3, 2);
						int liczba;
						if ((wartosc[1] < 48 || wartosc[1]>57) && (wartosc[0] < 48 || wartosc[0]>57)) {
							cout << endl << "Niepoprawna instrukcja!" << endl;
						}
						else {
							liczba = (wartosc[0] - 48) * 10 + (wartosc[1] - 48);
							LicznikRozkazow = liczba;
						}
					}

				}
			}
			else if (operacja == "CL") { //czyszczenie ekranu
				system("cls");
			}
			else if (operacja == "SP") { //stworz plik
				if (rozkaz.length() != 15 || rozkaz.substr(2, 1) != " " || rozkaz.substr(11, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string nazwa = rozkaz.substr(3, 8);
					string rozsz = rozkaz.substr(12, 3);
					dysk.create_file(nazwa, rozsz);
				}
			}
			else if (operacja == "ZP") { //zapisz do pliku
				if (rozkaz.length()<17 || rozkaz.substr(2, 1) != " " || rozkaz.substr(11, 1) != " " || rozkaz.substr(15, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string nazwa = rozkaz.substr(3, 8);
					string rozsz = rozkaz.substr(12, 3);
					string data = rozkaz.substr(16, rozkaz.length() - 16);

					dysk.write_file(nazwa, rozsz, data);
				}
			}
			else if (operacja == "UP") { //usun plik
				if (rozkaz.length() != 15 || rozkaz.substr(2, 1) != " " || rozkaz.substr(11, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string nazwa = rozkaz.substr(3, 8);
					string rozsz = rozkaz.substr(12, 3);
					dysk.delete_file(nazwa, rozsz);
				}
			}
			else if (operacja == "CP") { //czytanie z pliku
				if (rozkaz.length() != 15 || rozkaz.substr(2, 1) != " " || rozkaz.substr(11, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string nazwa = rozkaz.substr(3, 8);
					string rozsz = rozkaz.substr(12, 3);
					dysk.print_file(nazwa, rozsz);
				}
			}
			else if (operacja == "LP") { //lista plikow
				dysk.view_files();
			}
			else if (operacja == "NP") { //zmiana nazwy pliku
				if (rozkaz.length() != 28 || rozkaz.substr(2, 1) != " " || rozkaz.substr(11, 1) != " " || rozkaz.substr(15, 1) != " " || rozkaz.substr(24, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string nazwa = rozkaz.substr(3, 8);
					string rozsz = rozkaz.substr(12, 3);
					string nowa_nazwa = rozkaz.substr(16, 8);
					string nowe_rozsz = rozkaz.substr(25, 3);
					dysk.rename_file(nazwa, rozsz, nowa_nazwa, nowe_rozsz);
				}
			}
			else if (operacja == "DP") { //dopisac do pliku
				if (rozkaz.length()<17 || rozkaz.substr(2, 1) != " " || rozkaz.substr(11, 1) != " " || rozkaz.substr(15, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string nazwa = rozkaz.substr(3, 8);
					string rozsz = rozkaz.substr(12, 3);
					string dane = rozkaz.substr(16, rozkaz.length() - 16);
					dysk.append_file(nazwa, rozsz, dane);
				}
			}
			else if (operacja == "WP") { //wypisywanie dysku
				dysk.print_drive();
			}
			else if (operacja == "WF") { //wypisywanie fat
				dysk.stan_FAT();
			}
			else if (operacja == "WS") { //wypisywanie wybranego fat
				dysk.print_Jap();
			}
			else if (operacja == "DE") { //wypisz wpisy katalogowe
				dysk.directory_entry();
			}
			else if (operacja == "XW") { //pokaz wszystkie procesy
				wypiszListe(wszystkieProcesy);
			}
			else if (operacja == "PP") { //pokaz pamiec
				PamOperacyjna pamiec;
				pamiec.showRAM();
			}
			else if (operacja == "XP") { //pokaz drzewo procesow
				drzewoProcesow();
			}
			else if (operacja == "XG") { //pokaz gotowe procesy
				wypiszListe(Scheduler::ReadyProcessList);
			}
			else if (operacja == "XO") { //pokaz oczekujace procesy
				wypiszListe(oczekujaceProcesy);
			}
			else if (operacja == "XC") { //utworzenie procesu
				if (rozkaz.length()<4 || rozkaz.substr(2, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string rodzic = rozkaz.substr(3, rozkaz.length() - 3);
					int PID;
					string nazwa;

					if (rodzic[0] < 48 || rodzic[0]>57) {
						cout << endl << "Niepoprawna instrukcja!" << endl;
					}
					else {
						PID = (rodzic[0] - 48);
						PCB*parent = getProcess(PID);

						if (getProcess(PID) == nullptr) {
							cout << endl << "Nie znaleziono procesu" << endl;
						}
						else {
							if (rozkaz.length() == 4) {
								parent->fork();
							}
							else {
								if (rozkaz.substr(4, 1) != " ") {
									cout << endl << "Niepoprawna instrukcja!" << endl;
								}
								else {
									nazwa = rozkaz.substr(5, rozkaz.length() - 5);
									if (parent->fork(nazwa) == nullptr) {
										pomocnicza2 = 1;
									}
								}
							}
						}
					}
				}
			}
			else if (operacja == "XD") { //usuniecie procesu
				if (rozkaz.length() != 4 || rozkaz.substr(2, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string wartosc = rozkaz.substr(3, 1);
					int PID;

					if (wartosc[0] < 48 || wartosc[0]>57) {
						cout << endl << "Niepoprawna instrukcja!" << endl;
					}
					else {
						PID = (wartosc[0] - 48);

						kill(PID);
					}
				}
			}
			else if (operacja == "XN") { //znalezienie procesu o PID
				if (rozkaz.length() != 4 || rozkaz.substr(2, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string wartosc = rozkaz.substr(3, 1);
					int PID;

					if (wartosc[0] < 48 || wartosc[0]>57) {
						cout << endl << "Niepoprawna instrukcja!" << endl;
					}
					else {
						PID = (wartosc[0] - 48);
						if (getProcess(PID) == nullptr) {
							cout << endl << "Nie znaleziono procesu" << endl;
						}
						else {
							cout << endl << "Proces znaleziono" << endl;
						}
					}
				}
			}
			else if (operacja == "XR") { //czytanie komunikatu
				if (rozkaz.length() != 4 || rozkaz.substr(2, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string odkogo = rozkaz.substr(3, 1);
					int odkogoPID;
					if (odkogo[0] < 48 || odkogo[0]>57) {
						cout << endl << "Niepoprawna instrukcja!" << endl;
					}
					else {
						odkogoPID = odkogo[0] - 48;
						Message* a = PostOffice::receive(odkogoPID);
						if (a == nullptr) {
							cout << "Brak skrzynki!" << endl;
							pomocnicza2 = 1;
						}
						else {
							cout << "Komunikat ze skrzynki:" << a->getMesageText() << endl;
						}
					}
				}
			}
			else if (operacja == "XS") { //wysylanie komunikatu
				if (rozkaz.length() != 8 || rozkaz.substr(2, 1) != " " || rozkaz.substr(4, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string odbiorca = rozkaz.substr(3, 1);
					int recipientPID;
					string wartosc;

					if (odbiorca[0] < 48 || odbiorca[0]>57) {
						cout << endl << "Niepoprawna instrukcja!" << endl;
					}
					else {

						recipientPID = odbiorca[0] - 48;
						wartosc = rozkaz.substr(5, 3);

						string komunikat = wartosc.substr(0, 3);
						Message *wiadomosc = new Message(PID, komunikat);
						PostOffice::send(wiadomosc, recipientPID);
					}
				}
			}
			else if (operacja == "SK") {
				if (rozkaz.length() != 2) {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					PostOffice::showPostBox(PID);
					PostOffice::showPostOffice();
				}
			}
			else if (operacja == "ZS") { //zmiana stanu
				if (rozkaz.length() != 6 || rozkaz.substr(2, 1) != " " || rozkaz.substr(4, 1) != " ") {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					string proces = rozkaz.substr(3, 1);
					int procesPID;
					string stan;

					if (proces[0] < 48 || proces[0]>57) {
						cout << endl << "Niepoprawna instrukcja!" << endl;
					}
					else {
						procesPID = proces[0] - 48;
						stan = rozkaz.substr(5, 1);

						if (stan == "C") {
							zmienStan(procesPID, PCB::stanProcesu::Czekaj¹cy);
							Scheduler::Schedule();
						}
						else if (stan == "G") {
							zmienStan(procesPID, PCB::stanProcesu::Gotowy);
							Scheduler::Schedule();
						}
						else {
							cout << endl << "Niepoprawna instrukcja!" << endl;
						}
					}
				}
			}
			else if (operacja == "SE") { // wypisz procesy pod semaforami
				if (rozkaz.length() > 2) {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					MEMORY.wyswietlListe();
					semaphorePOST.wyswietlListe();
				}
			}
			else if (operacja == "SS") {// wypisz stany semaforow
				if (rozkaz.length() > 2) {
					cout << endl << "Niepoprawna instrukcja!" << endl;
				}
				else {
					if (MEMORY.isOpen() == true) {
						cout << endl << "Stan semafora MEMORY: otwarty." << endl;
					}
					else {
						cout << endl << "Stan semafora MEMORY: zamkniety." << endl;
					}
					if (semaphorePOST.isOpen() == true) {
						cout << endl << "Stan semafora POST: otwarty." << endl;
					}
					else {
						cout << endl << "Stan semafora POST: zamkniety." << endl;
					}
				}
			}
			else {
				cout << endl << "Nie rozpoznano polecenia" << endl;
				pomocnicza = 1;
				ZapiszRejestry();
			}
			if (pomocnicza == 0) {
				StanRejestrow();
				if (pomocnicza2 == 1) {
					PobierzRejestry();
				}
				if (instrukcja == "c"  &&pomocnicza2 == 0) {
					LicznikRozkazow = 1 + LicznikRozkazow + rozkaz.length();
				}
				if (operacja != "ZS" || pomocnicza2 == 0) {
					ZapiszRejestry();
				}
			}
		}
	}

	void boot() {
		system("cls");
		cout << "   @@@@@@@@@@@@@                          @@                               @@@@@           @@@@@    " << endl;
		cout << "   @@@@@@@@@@@@@                          @@                             @@@@@@@@@       @@@@@@@@@  " << endl;
		cout << "   @@@@     @@@@                          @@                            @@@     @@@     @@@     @@@ " << endl;
		cout << "             @@@                          @@                           @@@       @@@    @@       @@ " << endl;
		cout << "             @@@                          @@                           @@         @@@   @@       @@ " << endl;
		cout << "             @@@   @@@@@@@     @@@@@@@@@  @@    @@@@@@@  @@@      @@@ @@@         @@@   @@          " << endl;
		cout << "             @@@  @@@@@@@@@   @@@@@@@@@@@ @@   @@@@@@@@@ @@@     @@@  @@           @@   @@@@@       " << endl;
		cout << "             @@@  @@     @@   @@       @@ @@   @@     @@ @@@    @@@   @@           @@    @@@@@@@@   " << endl;
		cout << "             @@@  @@     @@   @@          @@   @@     @@ @@@   @@@    @@           @@      @@@@@@@  " << endl;
		cout << "             @@@  @@     @@   @@          @@   @@     @@ @@@  @@@     @@           @@          @@@@ " << endl;
		cout << "             @@@  @@@@@@@@@   @@          @@   @@@@@@@@@ @@@@@@@      @@           @@            @@@" << endl;
		cout << "             @@@  @@@@@@@@@   @@     @@@@ @@   @@@@@@@@@ @@@@@@@      @@           @@  @@         @@" << endl;
		cout << "  @@@        @@@  @@     @@   @@       @@ @@   @@     @@ @@@  @@@     @@@         @@@  @@@       @@@" << endl;
		cout << "   @@@      @@@@  @@     @@   @@       @@ @@   @@     @@ @@@   @@@     @@@       @@@    @@@      @@ " << endl;
		cout << "    @@@@@@@@@@@   @@     @@    @@@@@@@@@@ @@@@ @@     @@ @@@    @@@     @@@@@@@@@@@     @@@@@@@@@@@ " << endl;
		cout << "     @@@@@@@@@    @@     @@    @@@@@@@@@  @@@@ @@     @@ @@@     @@@      @@@@@@@         @@@@@@@@  " << endl << endl;
	}
};