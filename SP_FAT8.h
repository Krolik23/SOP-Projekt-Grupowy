#pragma once
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <math.h>
#include <windows.h>

using namespace std;


// OBS£UGA SYSTEMU PLIKÓW FAT8 - Last : 13.12 [20:24]
/*
FUNKCJE

void create_file(string name, string ext);					 - Stworzenie pliku
void view_files();											 - Wyœwietelenie drzewa plików
void write_file(string name, string ext, string data);		 - Zapis do pliku pustego
void delete_file(string name, string ext);					 - Usuwanie pliku
void print_file(string name, string ext);                    - Wydrukowanie/pokazanie zawartoœci pliku
void rename_file(string name, string ext, string newname);   - Zmiana nazwy pliku
void append_file(string name, string ext, string data);      - Dopisanie do pliku istniej¹cego
*/




struct KATALOG
{
	string name;	//Nazwa pliku
	string ext;		//Rozszerzenie
	bool status;	//Status
	int first_jap;	//Pierwszy Jap
	int size;		//Rozmiar
	bool apend;		//Czy zapisany?
	int dzien;		//Dzien
	int miesiac;	//Miesiac
	int rok;		//Rok
	int godzina;	//Godzina
	int minuta;		//Minuta
};


class DYSK
{
public:
	unsigned char dysk[1024];    //Dysk o pojemnosci 1024 bity
	int FATTable[32];			//Tablica FAT o pojemnoœci 32 bity
	KATALOG katalog[32];		//Wpisy katalogu glownego 
	int freespace = 992;		//Wolne miejsce

	DYSK()
	{
		FATTable[0] = -1;
		katalog[0].status = -1;
		dysk[0] = 'y'; // Poprawna inicjalizacja
		dysk[1] = 'y'; // Wolne miejsce
		dysk[2] = 'y'; //czy mozna dopisac katalog?
		dysk[3] = 'n'; //czy jest katalog?

		for (int i = 3; i < 1024; i++)  //Wypelnienie dysku 0
			dysk[i] = 0;

		for (int i = 1; i < 32; i++) //Zwalnanie sektorów FAT
			FATTable[i] = 0;


		for (int i = 1; i < 32; i++) {
			katalog[i].status = 0;
			katalog[i].apend = 0;
		}
	}

	void create_file(string name, string ext)
	{
		int temp;
		int temp2;
		SYSTEMTIME st;
		GetLocalTime(&st);


		if (dysk[1] == 'y' && dysk[2] == 'y' && name_exist(name, ext) == 1) {     //SPRAWDZANIE CZY JEST WOLNE MIEJSCE/WPIS KATALOGOWY I CZY PLIK O TEJ NAZWIE NIE ISTNIEJE
			temp = search_free_jap();    //Szukanie wolnego JAPA 
			temp2 = free_catalog();      //Szukanie wolnego katalogu

										 /* TWORZENIE KATALOGU  */
			katalog[temp2].name = name;
			katalog[temp2].status = 1;
			katalog[temp2].first_jap = temp;
			katalog[temp2].ext = ext;
			katalog[temp2].size = 0;
			katalog[temp2].apend = 0;
			katalog[temp2].godzina = st.wHour;
			katalog[temp2].minuta = st.wMinute;
			katalog[temp2].dzien = st.wDay;
			katalog[temp2].miesiac = st.wMonth;
			katalog[temp2].rok = st.wYear;
			FATTable[temp] = -1;


			cout << "Plik utworzony pomyslnie" << endl;;

			calculate_free();   //WOLNE MIEJSCE
			dysk[3] = 'y';      //CZY DYSK JEST UZYWANY?
		}
		else if (dysk[2] == 'n')
		{
			cout << "Katalog jest pelny!" << endl;
		}
		else if (dysk[1] == 'n')
		{
			cout << "Dysk jest pelny!" << endl;
		}
		else cout << "Plik o takiej nazwie juz istnieje" << endl;
	}

	void view_files()
	{
		int allsize = 0;
		int l = 0;
		cout << endl;
		cout << "  Directory of root:" << endl;
		cout << endl;
		cout << "\t" << "Name \t" << "\t" << "Ext. \t" << "Size \t" << "CreateData \t" << "Hour \t" << endl;
		for (int i = 1; i < 32; i++)
			if (katalog[i].status == 1)
			{
				cout << "\t" << katalog[i].name << " \t" << katalog[i].ext << "\t" << katalog[i].size << "\t" << katalog[i].rok << "-" << katalog[i].miesiac << "-" << katalog[i].dzien << "\t";
				view_hour(katalog[i].godzina);
				cout << ":";
				view_hour(katalog[i].minuta);
				cout << endl;
				l++;
				allsize += katalog[i].size;
			}
		if (dysk[3] == 'n')
		{
			cout << "Brak plikow w katalogu " << endl;
		}
		cout << endl;
		cout << "\t" << "\t" << l << " file(s)" << " \t" << allsize << " bytes" << endl;
		cout << "\t" << "\t" << "\t" << "\t" << 992 - allsize << " bytes free" << endl;
		cout << endl;
	}

	void write_file(string name, string ext, string data)
	{
		/* Zadeklarowanie potrzebnych zmiennych */
		int jap_pr;
		int japnext;
		float length;
		float count_jap;
		int i;
		int k;
		int l;
		length = data.length();
		count_jap = ceil(length / 32);

		/* Czy katalog wolny?  */
		int cat = what_catalog(name, ext);
		if (katalog[cat].apend == 0) {
			if (cat != -1)
				katalog[cat].apend = 1;


			if (file_jap(name, ext) != -1) {
				if (freespace > length) {
					jap_pr = file_jap(name, ext);
					i = jap_pr * 32;
					k = 0;
					l = i + 32;


					//DLA PIERWSZEGO JAPA
					for (i; i < l; i++) {
						if (k <= length - 1) {
							dysk[i] = data[k];
							k++;
						}
					}


					//DLA WIECEJ JAPOW
					if (count_jap > 1) {

						for (int j = 2; j <= count_jap; j++) {
							japnext = search_free_jap();
							FATTable[japnext] = -1;
							FATTable[jap_pr] = japnext;
							jap_pr = japnext;
							i = japnext * 32;
							l = i + 32;

							for (i; i < l; i++) {
								if (k <= length - 1)
								{
									dysk[i] = data[k];
									k++;
								}
							}
						}
					}
					cout << "Wpisanie do pliku pomyslne" << endl;
					new_size(name, ext, length);

				}
				else cout << "Za malo miejsca na dysku aby dopisac plik" << endl;
			}


			else cout << " Blad dopisywania! Plik nie istnieje" << endl;



		}
		else cout << "Nie mozna nadpisac danych" << endl;
		calculate_free();


	}


	void delete_file(string name, string ext)
	{

		int firstjap = file_jap(name, ext);
		int nextjap;
		int l = 1;
		int tab[32];
		tab[0] = firstjap;

		while (firstjap != -1)
		{
			nextjap = FATTable[firstjap];
			firstjap = nextjap;
			tab[l] = nextjap;
			l++;
		}
		l = l - 1;





		if (name_exist(name, ext) == 0) {
			for (int i = 0; i < l; i++)
			{
				int j = 32 * tab[i];
				int k = j + 32;
				for (j; j < k; j++)
				{
					dysk[j] = 0;
				}
				FATTable[tab[i]] = 0;
			}

			for (int i = 0; i < 32; i++)
			{
				if (katalog[i].name == name && katalog[i].ext == ext)
				{
					katalog[i].name = "";
					katalog[i].ext = "";
					katalog[i].size = 0;
					katalog[i].status = 0;
					katalog[i].first_jap = 0;
					katalog[i].apend = 0;
				}
			}
			calculate_free();
			cout << "Usuwanie pliku pomyslne" << endl;
		}
		else cout << "Plik nie istnieje" << endl;

	}


	void print_file(string name, string ext)
	{
		int firstjap = file_jap(name, ext);
		int nextjap;
		int l = 1;
		int tab[32];
		tab[0] = firstjap;


		if (firstjap != -1) {
			while (firstjap != -1)
			{
				nextjap = FATTable[firstjap];
				firstjap = nextjap;
				tab[l] = nextjap;
				l++;
			}
			l = l - 1;


			for (int i = 0; i < l; i++)
			{
				int j = 32 * tab[i];
				int k = j + 32;
				for (j; j < k; j++)
				{
					//if (dysk[j] == -1) break; TUT
					cout << dysk[j];
				}

			}
			cout << endl;

		}
		else cout << "Nie znaleziono podanego pliku" << endl;
	}

	void rename_file(string name, string ext, string newname, string newext)
	{
		if (name_exist(newname, newext) == 1) {
			if (file_jap(name, ext) != -1) {
				for (int i = 0; i < 32; i++)
				{
					if (katalog[i].name == name && katalog[i].ext == ext)
					{
						katalog[i].name = newname;
						katalog[i].ext = newext;
					}
				}
				cout << "Zmiana nazwy pomyslna" << endl;
			}
			else cout << "Nie znaleziono pliku" << endl;
		}
		else cout << "Plik o takiej nazwie juz istnieje" << endl;

	}

	void append_file(string name, string ext, string data)
	{
		if (file_jap(name, ext) != -1) {
			int firstjap = file_jap(name, ext);
			int nextjap;
			int lastjap;
			int l;
			int k;
			int i;
			int how;
			float length;
			int japnext;
			float count_jap;
			int cat;
			//			cout << "Pokaz mi nazwe, roz,data" << name << ext << data << endl;

			while (firstjap != -1)
			{
				nextjap = FATTable[firstjap];

				if (nextjap == -1)
				{
					lastjap = firstjap;
				}
				firstjap = nextjap;
			}

			cat = what_catalog(name, ext);
			how = free_in_jap(lastjap);
			//			cout << "How to: " << how << endl;
			length = data.length();
			count_jap = ceil((length - how) / 32);

			//WYPELNIAM STAREGO JAPA

			if (katalog[cat].apend == 1) {
				if (freespace > (length - how)) {
					new_size(name, ext, data.length());

					i = (lastjap * 32) + (32 - how);
					k = i + how;
					l = 0;
					//					cout << "Zaczynam dopis od" << i << "Koncze na" << k << endl;
					//					cout << "Pokaz mi na ktorym JAPIE dzialasz" << lastjap << endl;

					for (i; i < k; i++) {
						if (l < length)
							dysk[i] = data[l];
						l++;
					}

					for (int j = 1; j <= count_jap; j++) {
						japnext = search_free_jap();
						FATTable[japnext] = -1;
						FATTable[lastjap] = japnext;
						lastjap = japnext;
						i = japnext * 32;
						k = i + 32;
						//						cout << "Zaczynam dopis od" << i << "Koncze na" << k << endl;
						for (i; i < k; i++) {
							if (l < length)
							{

								dysk[i] = data[l];
								l++;
							}
						}
					}
					cout << "Dopisanie pomyslne" << endl;
					calculate_free();
				}

				else cout << "Za malo miejsca aby dopisac do pliku" << endl;
				calculate_free();
			}
			else cout << "Plik nie zostal wczesniej wypelniony" << endl;
		}

		else cout << "Plik o takiej nazwie nie istnieje" << endl;
	}

	void extended_FAT()
	{
		int temp = 0;
		while (temp != 4) {
			cout << endl;
			cout << endl;
			cout << endl;
			cout << "Uruchomiles diagnotyke sytemu plikow FAT" << endl;
			cout << endl;
			cout << "1 - Wypisz stan TablicFAT" << endl;
			cout << "2 - Wypisz caly dysk" << endl;
			cout << "3 - Wypisz zawartosc wskazanego JAP" << endl;
			cout << "4 - Wyjdz z diagnostyki" << endl;

			cin >> temp;
			switch (temp) {

			case 1:
			{	stan_FAT();
			break; }

			case 2:
			{print_drive();
			break;
			}

			case 3:
			{print_Jap();
			break;
			}


			}
		}
	}



	//MOJE WLASNE FUNKCJE

	void print_Jap()
	{
		int temp;
		cout << "Podaj JAP do wypisania" << endl;
		cin >> temp;

		int i = temp * 32;
		int k = i + 32;
		cout << "Wypisuje" << endl;
		for (i; i < k; i++)
			cout << dysk[i];

	}

	void new_size(string name, string ext, int size)
	{
		for (int i = 0; i < 32; i++) {
			if (katalog[i].name == name && katalog[i].ext == ext) {
				katalog[i].size += size;
				katalog[0].size += size;
			}
		}
	}



	int file_jap(string name, string ext)
	{

		for (int i = 0; i < 32; i++) {
			if (katalog[i].name == name && katalog[i].ext == ext)
				return katalog[i].first_jap;
		}
		return -1;
	}


	int free_in_jap(int nr_jap)
	{
		int i = nr_jap * 32;
		int k = i + 32;
		int l = 0;
		for (i; i < k; i++)
		{
			if (dysk[i] == 0)
				l++;
		}

		return l;
	}



	int jap_chain_free()
	{
		int i = 0;
		for (int i = 1; i < 32; i++)
		{
			if (FATTable[i] = 0);
			i++;
		}
		return i;

	}

	int what_catalog(string name, string ext)
	{
		for (int i = 0; i < 32; i++) {
			if (katalog[i].name == name && katalog[i].ext == ext)
			{
				return i;
			}
		}

		return -1;

	}

	int free_catalog()
	{
		for (int i = 0; i < 32; i++) {
			if (katalog[i].status == 0)
			{
				return i;
			}
		}

		return -1;
	}

	bool name_exist(string name, string ext)
	{
		for (int i = 0; i < 32; i++) {
			if (katalog[i].name == name && katalog[i].ext == ext)
			{
				return 0;
			}
		}

		return 1;


	}

	void calculate_free()
	{
		dysk[1] = 'n';
		dysk[2] = 'n';
		int l = 0;
		for (int i = 1; i < 32; i++)
		{
			if (FATTable[i] == 0)
			{
				dysk[1] = 'y';
				l++;
			}

			if (katalog[i].status == 0)
				dysk[2] = 'y';
		}

		freespace = l * 32;
	}





	int search_free_jap()
	{
		for (int i = 1; i < 32; i++)
		{
			if (FATTable[i] == 0)
				return i;
		}
		return -1;
	}

	void stan_FAT()
	{
		cout << endl;
		string temp;
		for (int i = 0; i < 32; i++)
		{
			cout << "Numer FAT : " << i << "\t" << "STAN :" << FATTable[i] << endl;

		}
		cout << endl;
	}

	void print_drive()
	{
		for (int i = 0; i < 1024; i++) {
			cout << dysk[i];
		}
	}

	void print_to_file(string directory)
	{
		fstream plik(directory, ios::out);
		{
			for (int i = 0; i < 1024; i++) {
				plik << i << " " << dysk[i] << endl;
			}

			plik.close();
			cout << "Dysk wyeksportowany" << endl;

		}
	}


	void view_hour(int a)
	{
		if (a >= 10)
			cout << a;
		else
		{
			if (a == 0) cout << "00";
			if (a == 1) cout << "01";
			if (a == 2) cout << "02";
			if (a == 3) cout << "03";
			if (a == 4) cout << "04";
			if (a == 5) cout << "05";
			if (a == 6) cout << "06";
			if (a == 7) cout << "07";
			if (a == 8) cout << "08";
			if (a == 9) cout << "09";
		}
	}

	void directory_entry()
	{
		cout << endl;
		cout << "  Directory of root:" << endl;
		cout << endl;
		cout << "\t" << "Name \t" << "\t" << "Ext. \t" << "Size \t" << "FJap \t" << "Write? \t" << "Numer" << endl;;
		for (int i = 1; i < 32; i++)
			if (katalog[i].status == 1)
			{
				cout << "\t" << katalog[i].name << " \t" << katalog[i].ext << "\t" << katalog[i].size << "\t" << katalog[i].first_jap << "\t" << katalog[i].apend << "\t" << i << endl;
			}
		cout << endl;

	}


};

