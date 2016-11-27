#pragma once
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <cmath>
#include <iomanip>
#include <math.h>
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
	string ext;
	bool status;
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

		for (int i = 3; i < 1028; i++)  //wypelnienie dysku 0
			dysk[i] = 0;

		for (int i = 1; i < 32; i++) //Zwalnanie sektorów FAT
			FATTable[i] = 0;


		for (int i = 1; i < 32; i++)
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
			katalog[temp2].last_jap = temp;
			katalog[temp2].ext = ext;
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
		cout << endl;
		cout << "  Directory of root:" << endl;
		cout << endl;
		cout << "\t" << "Name \t" << "Ext. \t" << "Size \t" << endl;
		for (int i = 0; i <= 32; i++)
			if (katalog[i].status == 1)
			{
				cout << "\t" << katalog[i].name << "\t" << katalog[i].ext << "\t" << katalog[i].size << endl;
			}
		cout << endl;
	}

	void write_file(string name, string data, string ext)
	{
		int jap_pr;
		int japnext;
		float length;
		float count_jap;
		int i;
		int k;
		int l;
		length = data.length();
		new_size(name, ext, length);
		count_jap = ceil(length / 32);
		//SPRWADZ CZY PLIK JEST
		if (file_jap(name, ext) != 0) {
			jap_pr = file_jap(name, ext);
			i = jap_pr * 32;
			k = 0;
			l = i + 32;

			//DLA PIERWSZEGO JAPA
			for (i; i<l; i++) {
				if (k <= length - 1) {
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
		}
		else cout << " Blad dopisywania plik nie istnieje" << endl;

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
			}
		}

	}

	void print_file(string name, string ext)
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


		for (int i = 0; i < l; i++)
		{
			int j = 32 * tab[i];
			int k = j + 32;
			for (j; j < k; j++)
			{
				cout << dysk[j] << " ";
			}
		}



	}

	void rename_file(string name, string ext, string newname, string newext)
	{
		for (int i = 0; i < 32; i++)
		{
			if (katalog[i].name == name && katalog[i].ext == ext)
			{
				katalog[i].name = newname;
				katalog[i].ext = newext;
			}
		}

	}

	void append_file(string name, string ext, string data)
	{
		int firstjap = file_jap(name, ext);
		new_size(name, ext, data.length());
		int nextjap;
		int lastjap;
		int l;
		int k;
		int i;
		int how;
		float length;
		int japnext;
		float count_jap;



		while (firstjap != -1)
		{
			nextjap = FATTable[firstjap];

			if (nextjap == -1)
			{
				lastjap = firstjap;
			}
			firstjap = nextjap;
		}


		how = free_in_jap(lastjap);
		length = data.length();
		count_jap = ceil((length - how) / 32);

		//WYPELNIAM STAREGO JAPA


		i = (lastjap * 32) + (32 - how);
		k = i + how;
		l = 0;


		for (i; i < k; i++) {
			if (l < length)
				dysk[i] = data[l];
			l++;
		}


		if (length>how) {

			for (int j = 1; j <= count_jap; j++) {
				japnext = search_free_jap();
				FATTable[japnext] = -1;
				FATTable[lastjap] = japnext;
				lastjap = japnext;
				i = japnext * 32;
				k = i + 32;

				for (i; i < k; i++) {
					if (l < length)
					{

						dysk[i] = data[l];
						l++;
					}
				}
			}
		}

	}




	//MOJE WLASNE FUNKCJE

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
				return i;
		}
		return 0;
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


	int free_catalog()
	{
		for (int i = 0; i <= 32; i++) {
			if (katalog[i].status == 0)
			{
				return i;
			}
		}

		return 0;
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
			if (FATTable[i] == 0)
				return i;
		}
		return 0;
	}

	void stan_FAT()
	{
		string temp;
		for (int i = 1; i < 32; i++)
		{
			cout << "Numer JAP : " << i << "/t STAN JAP :" << FATTable[i] << endl;

		}
	}

	void print_drive()
	{
		for (int i = 0; i < 1028; i++) {
			cout << dysk[i] << " ";
		}
	}

	void print_to_file(string directory)
	{
		fstream plik(directory, ios::out);
		{
			for (int i = 0; i < 1028; i++) {
				plik << i << " " << dysk[i] << endl;
			}

			plik.close();

		}
	}


};