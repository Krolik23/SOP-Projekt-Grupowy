#include "Procesy.h"


int free_pid = 1;//PID nigdy sie nie powtarza, kazdy nowo utworzony dostaje free_pid ktore jest potem inkrementowane

				 /*LISTY I WEKTORY PROCESOW*/
list<PCB*> wszystkieProcesy;
list<PCB*> oczekujaceProcesy;
vector<Oczekiwanie> poWait;
vector<int> poExit;
list<PCB*> gotoweProcesy;

PCB::PCB()
{
}

PCB::~PCB()
{
}

int PCB::fork()
{
	auto nowy_proces = new PCB();

	// USTAWIAM STAN PROCESU NA NOWY ORAZ WST�PNE INFORMACJE W PROCESIE

	nowy_proces->Stan = PCB::stanProcesu::Nowy;
	nowy_proces->idRodzica = this->PID;

	// NADAJE PID NOWEMU PROCESSOWI

	nowy_proces->PID = free_pid;
	free_pid++;

	idPotomnych.push_back(nowy_proces->PID);	//DODANIE POTOMKA DO LISTY RODZICA

	nowy_proces->timer = 0;

	nowy_proces->A = 0;
	nowy_proces->B = 0;
	nowy_proces->C = 0;
	nowy_proces->licznikRozkazow = 0;

	/*REZERWOWANIE PAMIECI*/


	/*-------------------*/
	wszystkieProcesy.push_back(nowy_proces);
	return 0;
}

void PCB::wait(int ID)
{
	// SPRAWDZAMY CZY PROCES NA KT�RYM CHCEMY WYWO�A� WAITPIDA ZNAJDUJE SI� NA LI�CIE PRZECHOWUJ�CEJ PROCESY KT�RE SI� WYKONA�Y ( WYKONA�Y PROCESUR� EXIT() )
	int index = -1;
	for (int i = 0; i < poExit.size(); i++)
		if (poExit.at(i) == PID) { index = i; break; }

	// PROCES POTOMNY NA KT�RYM WYKONUJEMY WAITPIDA WYKONA� WCZE�NIEJ EXIT
	if (index >= 0)
	{
		// ZNAJDUJEMY NA LISCIE PROCESOW POTOMNYCH AKTUALNIE WYKONYWUJACEGO WAITPIDA PROCESU PROCES O ZDEFINIOWANYM PIDZIE
		PCB* wsk = getProcess(ID);

		// DZIALAMY TYLKO JESLI PROCES KTORY WYKONUJE WAITPIDA MA PROCES POTOMNY O PIDZIE KTORY PODAJEMY JAKO ARGUMENT WAITPIDA
		if (wsk->idRodzica == this->PID)
		{
			// ZMIENIAMY STAN PROCESU NA ZAKO�CZONY
			wsk->Stan = stanProcesu::Zako�czony;

			// ZABIJAMY PROCES CZYLI: ZNAJDUJEMY GO NA LI�CIE CYKLICZNEJ WSZYSTKICH PROCES�W, ZWALNIAMY ZAJMOWAN� PRZEZ NIEGO PAMI��, USUWAMY GO Z LISTY CYKLICZNEJ WSZYSTKICH PROCES�W, INIT_TASK ADOPTUJE WSZYSTKIE JEGO POTOMNE
			kill(wsk->PID);

			// USUWAMY INFORMACJ� O ZAKO�CZENIU SI� PROCESU Z LISTY poExit PO USUNI�CIU PROCESU
			poExit.erase((poExit.begin() + index));
		}
	}
	// PROCES POTOMNY NA KT�RYM WYWO�UJEMY WAITPIDA NIE WYWO�A� WCZE�NIEJ METODY EXIT()
	else
	{

		// STAN PROCESU MACIERZYSTEGO KT�RY WYKONA� WAITPIDA NA PROCESIE POTOMNYM KT�RY NIE WYKONA� EXITA ZMIENIAMY NA CZEKAJ�CY
		this->Stan = stanProcesu::Czekaj�cy;

		// TWORZYMY SYGNAL TEGO ZE OBECNY PROCES CZEKA NA WYKONANIE SI� PROCESU O PIDZIE PODANYM JAKO ARGUMENT WAITPIDA I ZGLASZAMY TAKIE OCZEKIWANIE DODAJAC JE DO ODPOWIEDNIEJ LISTY
		Oczekiwanie zglaszamNowe = { this->PID, PID };
		poWait.push_back(zglaszamNowe);
	}
}

bool PCB::exit()
{
	bool Usuniety = false;

	int index = -1;
	for (int i = 0; i < poWait.size(); i++)
		if (poWait.at(i).naKogo == this->PID) { index = i; break; }

	if (index >= 0)
	{
		int naIlePotomnychCzeka = 0; bool czyCzekaNaNiego = false; int indexSygnalu = 0;
		for (int i = 0; i < poWait.size(); i++)
		{
			if (poWait.at(i).kto == this->idRodzica)
			{
				++naIlePotomnychCzeka;
				if (poWait.at(i).naKogo == this->PID) { czyCzekaNaNiego = true; indexSygnalu = i; }
			}
		}

		if (czyCzekaNaNiego && naIlePotomnychCzeka == 1)
		{
			PCB* Rodzicwsk = getProcess(this->idRodzica);
			Rodzicwsk->Stan = PCB::stanProcesu::Gotowy;
			gotoweProcesy.push_back(Rodzicwsk);

			poWait.erase((poWait.begin() + indexSygnalu));

		}
		else if (czyCzekaNaNiego)
		{
			poWait.erase((poWait.begin() + indexSygnalu));
		}

		Usuniety = true;

		kill(this->PID);
	}
	else
	{
		this->Stan = stanProcesu::Zombie;

		poExit.push_back(this->PID);
	}

	// ZWRACAMY FLAGE INFORMUJACA O TYM CZY DOSZLO DO USUNIECIA PROCESU
	return Usuniety;
}

bool czyDziecko(int PID, int czyRodzicID)
{
	PCB* temp = nullptr;
	for (list<PCB*>::iterator iter = wszystkieProcesy.begin();
		iter != wszystkieProcesy.end(); ++iter)
	{
		temp = *iter;
		if (temp->PID == PID)
			break;

	}
	if (temp->idRodzica == czyRodzicID)
		return true;

	else
		return false;
}

void zmienStan(int ID, PCB::stanProcesu nowyStan)
{
	for (list<PCB*>::iterator iter = wszystkieProcesy.begin(); iter != wszystkieProcesy.end(); ++iter)
	{
		if ((*iter)->PID == ID) {
			switch (nowyStan)
			{
			case PCB::stanProcesu::Aktywny:
				if ((*iter)->Stan == PCB::stanProcesu::Czekaj�cy) {
					for (list<PCB*>::iterator it = oczekujaceProcesy.begin(); it != oczekujaceProcesy.end(); /*increment in body*/)
					{
						if ((*it)->PID == ID) //jesli ID sie zgadza usuni�cie wska�nika z listy
						{
							it = oczekujaceProcesy.erase(it);
							break;
						}
						else
						{
							++it;
						}
					}
				}
				else if ((*iter)->Stan == PCB::stanProcesu::Gotowy) {
					for (list<PCB*>::iterator it = oczekujaceProcesy.begin(); it != oczekujaceProcesy.end(); /*increment in body*/)
					{
						if ((*it)->PID == ID) //jesli ID sie zgadza usuni�cie wska�nika z listy
						{
							it = oczekujaceProcesy.erase(it);
							break;
						}
						else
						{
							++it;
						}
					}
				}
				/*NA KONIEC MOZNA SPOKOJNIE ZMIENIC STAN*/
				(*iter)->Stan = PCB::stanProcesu::Aktywny;
				break;

			case PCB::stanProcesu::Nowy:
				if ((*iter)->Stan == PCB::stanProcesu::Czekaj�cy) {
					for (list<PCB*>::iterator it = oczekujaceProcesy.begin(); it != oczekujaceProcesy.end(); /*increment in body*/)
					{
						if ((*it)->PID == ID) //jesli ID sie zgadza usuni�cie wska�nika z listy
						{
							it = oczekujaceProcesy.erase(it);
							break;
						}
						else
						{
							++it;
						}
					}
				}
				else if ((*iter)->Stan == PCB::stanProcesu::Gotowy) {
					for (list<PCB*>::iterator it = oczekujaceProcesy.begin(); it != oczekujaceProcesy.end(); /*increment in body*/)
					{
						if ((*it)->PID == ID) //jesli ID sie zgadza usuni�cie wska�nika z listy
						{
							it = oczekujaceProcesy.erase(it);
							break;
						}
						else
						{
							++it;
						}
					}
				}
				/*NA KONIEC MOZNA SPOKOJNIE ZMIENIC STAN*/
				(*iter)->Stan = PCB::stanProcesu::Nowy;
				break;

			case PCB::stanProcesu::Gotowy:
				if ((*iter)->Stan == PCB::stanProcesu::Czekaj�cy) {
					for (list<PCB*>::iterator it = oczekujaceProcesy.begin(); it != oczekujaceProcesy.end(); /*increment in body*/)
					{
						if ((*it)->PID == ID) //jesli ID sie zgadza usuni�cie wska�nika z listy
						{
							it = oczekujaceProcesy.erase(it);
							break;
						}
						else
						{
							++it;
						}
					}
				}

				(*iter)->Stan = PCB::stanProcesu::Gotowy;
				gotoweProcesy.push_back(*iter);
				break;

			case PCB::stanProcesu::Czekaj�cy:
				if ((*iter)->Stan == PCB::stanProcesu::Gotowy) {
					for (list<PCB*>::iterator it = oczekujaceProcesy.begin(); it != oczekujaceProcesy.end(); /*increment in body*/)
					{
						if ((*it)->PID == ID) //jesli ID sie zgadza usuni�cie wska�nika z listy
						{
							it = oczekujaceProcesy.erase(it);
							break;
						}
						else
						{
							++it;
						}
					}
				}
				(*iter)->Stan = PCB::stanProcesu::Czekaj�cy;
				break;

			case PCB::stanProcesu::Zako�czony:
				if ((*iter)->Stan == PCB::stanProcesu::Czekaj�cy) {
					for (list<PCB*>::iterator it = oczekujaceProcesy.begin(); it != oczekujaceProcesy.end(); /*increment in body*/)
					{
						if ((*it)->PID == ID) //jesli ID sie zgadza usuni�cie wska�nika z listy
						{
							it = oczekujaceProcesy.erase(it);
							break;
						}
						else
						{
							++it;
						}
					}
				}
				else if ((*iter)->Stan == PCB::stanProcesu::Gotowy) {
					for (list<PCB*>::iterator it = oczekujaceProcesy.begin(); it != oczekujaceProcesy.end(); /*increment in body*/)
					{
						if ((*it)->PID == ID) //jesli ID sie zgadza usuni�cie wska�nika z listy
						{
							it = oczekujaceProcesy.erase(it);
							break;
						}
						else
						{
							++it;
						}
					}
				}
				(*iter)->Stan = PCB::stanProcesu::Zako�czony;
				break;

			default: break;
			}
			break;
		}
	}
}

PCB * getProcess(int ID)
{
	PCB* temp;
	for (list<PCB*>::iterator iter = wszystkieProcesy.begin();
		iter != wszystkieProcesy.end(); ++iter)
	{
		temp = *iter;
		if (temp->PID == ID)
			return temp;

	}
	return nullptr;

}

void wypiszListe(list<PCB*> lista)	//ARGUMENTEM MOZE BYC: wszystkieProcesy, gotoweProcesy, oczekujaceProcesy
{
	cout << "ID procesow znajdujacych sie na liscie: ";
	for (list<PCB*>::iterator iter = lista.begin();
		iter != lista.end(); ++iter)
	{
		cout << (*iter)->PID << ", ";
	}
	cout << endl;
}

void init()
{
	auto INIT = new PCB();
	INIT->PID = 0; INIT->idRodzica = NULL;
	INIT->Stan = PCB::stanProcesu::Gotowy;
	wszystkieProcesy.push_back(INIT);
	gotoweProcesy.push_back(INIT);
}

bool kill(int ID)
{
	bool Usuniety = false;

	for (list<PCB*>::iterator iter = wszystkieProcesy.begin(); iter != wszystkieProcesy.end(); ++iter)
	{
		if ((*iter)->PID == ID)
		{
			//zwolnienie pamieci ??
			int naIlePotomnychCzeka = 0; bool czyCzekaNaNiego = false; int indexSygnalu = 0;
			for (int i = 0; i < poWait.size(); i++)
			{
				if (poWait.at(i).kto == (*iter)->idRodzica)
				{
					++naIlePotomnychCzeka;
					if (poWait.at(i).naKogo == (*iter)->PID) { czyCzekaNaNiego = true; indexSygnalu = i; }
				}
			}

			if (czyCzekaNaNiego && naIlePotomnychCzeka == 1)
			{
				PCB *wskk = getProcess((*iter)->idRodzica);
				wskk->Stan = PCB::stanProcesu::Gotowy;
				gotoweProcesy.push_back(wskk);

				poWait.erase((poWait.begin() + indexSygnalu));
			}
			else if (czyCzekaNaNiego)
			{
				poWait.erase((poWait.begin() + indexSygnalu));
			}

			for (list<PCB*>::iterator it = wszystkieProcesy.begin(); it != wszystkieProcesy.end(); /*increment in body*/)
			{
				if ((*it)->PID == ID) //jesli ID sie zgadza usuni�cie wska�nika z listy
				{
					it = wszystkieProcesy.erase(it);
					break;
				}
				else
				{
					++it;
				}
			}

			/*SPRAWDZAMY CZY USUWANY PROCES MA PROCESY POTOMNE - JE�LI TAK, ZOSTAN� ONE ZAADOPTOWANE PRZEZ PROCES INIT*/

			if ((*iter)->idPotomnych.size() != 0)
			{
				PCB* INITwsk = getProcess(0);
				for (int i = 0; i < (*iter)->idPotomnych.size(); i++)
				{
					PCB *wsk = getProcess((*iter)->idPotomnych.at(i));
					wsk->idRodzica = 0;
					INITwsk->idPotomnych.push_back(wsk->PID);

				}

			}

			Usuniety = true;

			break;
		}
	}

	return Usuniety;
}