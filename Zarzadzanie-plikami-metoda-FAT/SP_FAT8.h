#pragma once
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
using namespace std;


// OBS£UGA SYSTEMU PLIKÓW FAT32 beta 0.1
/*
FUNKCJE do wywo³ania

void create_file(string name, string ext, string ext);   - Stworzenie pliku
void view_files();										 - Wyœwietelenie drzewa plików (FAT)
void write_file(string name, string ext, string data);   - Zapis do pliku pustego
void delete_file(string name, string ext);               - Usuwanie pliku
void print_file(string name, string ext);                - Wydrukowanie/pokazanie zawartoœci pliku
void rename_file(string name, string ext, string newname); - Zmiana nazwy pliku
void append_file(string name, string ext, string data); //IN PROGRESS    -Dopisanie do pliku istniej¹cego
string execute_file (string name, string ext); -----> Wykonanie rozkazów z pliku? Raczej nie bêdzie. Konrad?

*/




struct KATALOG
{
	string name;
	bool status; //status wpisu
	int first_jap;
	int last_jap;
	int size;
};


class DYSK
{
public:
	unsigned char dysk[1028];
	int FATTable[32];
	KATALOG katalog[32];

	DYSK()
	{
		FATTable[0] = -1;
		dysk[0] = 'y'; // Poprawna inicjalizacja
		dysk[1] = 'y'; // Wolne miejsce
		dysk[2] = 'y'; //czy mozna dopisac katalog?

		for (int i = 2; i <= 1028; i++)  //wypelnienie dysku 0
			dysk[i] == 0;

		for (int i = 1; i <= 32; i++) //Zwalnanie sektorów FAT
			FATTable[i] = 0;

		for (int i = 0; i <= 32; i++)
			katalog[i].status = 0;
	}

	void create_file(string name, string ext)
	{
		int temp;
		int temp2;

		if (dysk[1] == 'y' && dysk[2] == 'y') {
			temp = search_free_jap();
			temp2 = free_catalog();

			katalog[temp2].name = name;
			katalog[temp2].status = 1;
			katalog[temp2].first_jap = temp;
			katalog[temp2].size = 0;
			FATTable[temp] = -1;

			cout << "Plik utworzony pomyslnie" << endl;;
			calculate_free();
		}
		else
		{
			cout << "B³¹d! Katalog nie zosta³ utworzony! " << endl;
			if (dysk[1] = 'y')
				cout << "Brak miejsca na wpis pliku" << endl;
			else cout << "Brak miejsca na dysku!" << endl;
		}
	}

	void view_files()
	{
		cout << "Zawartosc katalogu glowego :" << endl;
		cout << endl;
		cout << endl;
		cout << " \t Name \t" << "Rozmiar \t" << endl;
		for (int i = 0; i <= 32; i++)
			if (katalog[i].status == 1)
			{
				cout << "\t" << katalog[i].name << "\t" << "1" << endl;
			}
	}

	void write_file(string name, string data, string ext)
	{
		int jap_pr;
		int japnext;
		int length;
		int count_jap;
		int i;
		int k;
		int l;
		length = data.length();
		count_jap = length / 32;

		//SPRWADZ CZY PLIK JEST
		if (file_jap(name) != 0) {
			jap_pr = file_jap(name);
			i = jap_pr * 32 + 1;
			k = 0;
			l = i + 32;

			//DLA PIERWSZEGO JAPA
			for (i; i<l; i++) {
				if (k <= length) {
					dysk[i] = data[k];
					k++;
				}
			}

			//DLA WIECEJ JAPOW
			if (count_jap>1) {

				for (int j = 2; j <= count_jap; j++) {
					japnext = search_free_jap();
					FATTable[japnext] = -1;
					FATTable[jap_pr] = japnext;
					jap_pr = japnext;
					i = japnext * 32 + 1;
					l = i + 32;

					for (i; i < l; i++) {
						if (k <= length)
						{
							dysk[i] = data[k];
							k++;
						}
					}
				}
			}
		}
		else cout << " B³¹d dopisywania plik nie istnieje" << endl;

	}


	void delete_file(string name, string ext)
	{
		int firstjap = file_jap(name);
		int nextjap;
		int l = 1;

		while (firstjap == -1)
		{
			nextjap = FATTable[firstjap];
			firstjap = nextjap;
			l++;
		}

		firstjap = file_jap(name);
		int *tablica = new int[l];

		tablica[0] = firstjap;
		for (int i = 1; i < l; i++) {
			tablica[i] = FATTable[firstjap];
			firstjap = tablica[i];
		}


		for (int i = 0; i <= l; i++)
		{
			int j = 32 * tablica[i];
			int k = j + 32;
			for (j; j < 32; j++)
			{
				dysk[j] = 0;
			}
			FATTable[tablica[i]] = -1;
		}

		for (int i = 0; i < 32; i++)
		{
			if (katalog[i].name == name)
			{
				katalog[i].name = "";
				katalog[i].size = 0;
				katalog[i].status = 0;
				katalog[i].first_jap = -1;
			}
		}

	}

	void print_file(string name, string ext)
	{
		int firstjap = file_jap(name);
		int nextjap;
		int l = 1;

		while (firstjap == -1)
		{
			nextjap = FATTable[firstjap];
			firstjap = nextjap;
			l++;
		}

		firstjap = file_jap(name);
		int *tablica = new int[l];

		tablica[0] = firstjap;
		for (int i = 1; i < l; i++) {
			tablica[i] = FATTable[firstjap];
			firstjap = tablica[i];
		}


		for (int i = 0; i <= l; i++)
		{
			int j = 32 * tablica[i];
			int k = j + 32;
			for (j; j < 32; j++)
			{
				cout << dysk[j];
			}
		}



	}

	void rename_file(string name, string ext, string newname, string newext)
	{
		for (int i = 0; i < 32; i++)
		{
			if (katalog[i].name == name)
			{
				katalog[i].name = newname;
			}
		}

	}

	void append_file(string name, string ext, string data)
	{



	}

	//MOJE WLASNE FUNKCJE

	int file_jap(string name)
	{
		for (int i = 0; i <= 32; i++) {
			if (katalog[i].name == name)
				return i;
		}
		return 0;
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


	int free_catalog()
	{
		for (int i = 0; i <= 32; i++)
			if (katalog[i].status == 0)
			{
				return i;
			}
			else return -1;
	}


	void calculate_free()
	{
		dysk[1] = 'n';
		dysk[2] = 'n';
		for (int i = 1; i < 32; i++)
		{
			if (FATTable[i] == 0)
				dysk[1] = 'y';
			if (katalog[i].status == 0)
				dysk[2] = 'y';
		}

	}



	int search_free_jap()
	{
		for (int i = 1; i < 32; i++)
		{
			if (FATTable[i] = 0);
			return i;
		}
		return 0;
	}



};