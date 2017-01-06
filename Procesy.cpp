#include "Procesy.h"
#include "Scheduler.h"
#include "PamOperacyjna.h"
#include "PostOffice.h"

int free_pid = 1;//PID nigdy sie nie powtarza, kazdy nowo utworzony dostaje free_pid ktore jest potem inkrementowane

				 /*LISTY I WEKTORY PROCESOW*/
list<PCB*> wszystkieProcesy;
list<PCB*> oczekujaceProcesy;
vector<Oczekiwanie> poWait;
vector<int> poExit;
//list<PCB*> gotoweProcesy;
PamOperacyjna pam;

PCB::PCB()
{
}

PCB::~PCB()
{
}


PCB* PCB::fork()
{
	auto nowy_proces = new PCB();



	// USTAWIAM STAN PROCESU NA NOWY ORAZ WSTÊPNE INFORMACJE W PROCESIE

	nowy_proces->Stan = PCB::stanProcesu::Nowy;
	nowy_proces->idRodzica = this->PID;

	// NADAJE PID NOWEMU PROCESSOWI

	nowy_proces->PID = free_pid;
	free_pid++;

	idPotomnych.push_back(nowy_proces->PID);    //DODANIE POTOMKA DO LISTY RODZICA

	nowy_proces->timer = 0;
	nowy_proces->tau = 0;
	nowy_proces->tt = 0;

	nowy_proces->A = 0;
	nowy_proces->B = 0;
	nowy_proces->C = 0;
	nowy_proces->licznikRozkazow = 0;
	nowy_proces->base = -1;
	
	wszystkieProcesy.push_back(nowy_proces);
	zmienStan(nowy_proces->PID, stanProcesu::Gotowy);
	if (nowy_proces->PID == 1) nowy_proces->tau = 10;
	return nowy_proces;
}

PCB* PCB::fork(string p)
{
	auto nowy_proces = new PCB();

	nowy_proces->Stan = PCB::stanProcesu::Nowy;
	nowy_proces->idRodzica = this->PID;

	nowy_proces->PID = free_pid;
	free_pid++;

	nowy_proces->timer = 0;
	nowy_proces->tau = 0;
	nowy_proces->tt = 0;

	nowy_proces->A = 0;
	nowy_proces->B = 0;
	nowy_proces->C = 0;
	nowy_proces->licznikRozkazow = 0;
	nowy_proces->base = NULL;

	nowy_proces->fileName = p;

     if (pam.XA(p, nowy_proces)) {
		idPotomnych.push_back(nowy_proces->PID);    //DODANIE POTOMKA DO LISTY RODZICA

		wszystkieProcesy.push_back(nowy_proces);
		zmienStan(nowy_proces->PID, stanProcesu::Gotowy);
		if (nowy_proces->PID == 1) nowy_proces->tau = 10;
		return nowy_proces;
	}
	 else {
		 free_pid--;
		 delete nowy_proces;
		 return nullptr;
	 }
}

void PCB::wait(int ID)
{
	// SPRAWDZAMY CZY PROCES NA KTÓRYM CHCEMY WYWO£AÆ WAITPIDA ZNAJDUJE SIÊ NA LIŒCIE PRZECHOWUJ¥CEJ PROCESY KTÓRE SIÊ WYKONA£Y ( WYKONA£Y PROCESURÊ EXIT() )
	int index = -1;
	for (int i = 0; i < poExit.size(); i++)
		if (poExit.at(i) == PID) { index = i; break; }

	// PROCES POTOMNY NA KTÓRYM WYKONUJEMY WAITPIDA WYKONA£ WCZEŒNIEJ EXIT
	if (index >= 0)
	{
		// ZNAJDUJEMY NA LISCIE PROCESOW POTOMNYCH AKTUALNIE WYKONYWUJACEGO WAITPIDA PROCESU PROCES O ZDEFINIOWANYM PIDZIE
		PCB* wsk = getProcess(ID);

		// DZIALAMY TYLKO JESLI PROCES KTORY WYKONUJE WAITPIDA MA PROCES POTOMNY O PIDZIE KTORY PODAJEMY JAKO ARGUMENT WAITPIDA
		if (wsk->idRodzica == this->PID)
		{
			// ZMIENIAMY STAN PROCESU NA ZAKOÑCZONY
			//wsk->Stan = stanProcesu::Zakoñczony;
			zmienStan(wsk->PID, stanProcesu::Zakoñczony);

			// ZABIJAMY PROCES CZYLI: ZNAJDUJEMY GO NA LIŒCIE CYKLICZNEJ WSZYSTKICH PROCESÓW, ZWALNIAMY ZAJMOWAN¥ PRZEZ NIEGO PAMIÊÆ, USUWAMY GO Z LISTY CYKLICZNEJ WSZYSTKICH PROCESÓW, INIT_TASK ADOPTUJE WSZYSTKIE JEGO POTOMNE
			kill(wsk->PID);

			// USUWAMY INFORMACJÊ O ZAKOÑCZENIU SIÊ PROCESU Z LISTY poExit PO USUNIÊCIU PROCESU
			poExit.erase((poExit.begin() + index));
		}
	}
	// PROCES POTOMNY NA KTÓRYM WYWO£UJEMY WAITPIDA NIE WYWO£A£ WCZEŒNIEJ METODY EXIT()
	else
	{

		// STAN PROCESU MACIERZYSTEGO KTÓRY WYKONA£ WAITPIDA NA PROCESIE POTOMNYM KTÓRY NIE WYKONA£ EXITA ZMIENIAMY NA CZEKAJ¥CY
		zmienStan(this->PID, stanProcesu::Czekaj¹cy);
		//this->Stan = stanProcesu::Czekaj¹cy;

		// TWORZYMY SYGNAL TEGO ZE OBECNY PROCES CZEKA NA WYKONANIE SIÊ PROCESU O PIDZIE PODANYM JAKO ARGUMENT WAITPIDA I ZGLASZAMY TAKIE OCZEKIWANIE DODAJAC JE DO ODPOWIEDNIEJ LISTY
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
			//Rodzicwsk->Stan = PCB::stanProcesu::Gotowy;
			zmienStan(Rodzicwsk->PID, stanProcesu::Gotowy);
			//gotoweProcesy.push_back(Rodzicwsk);
			//Scheduler::ReadyProcessList.push_back(Rodzicwsk);


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
		
		if ((*iter)->PID == ID)
		{
		
			switch (nowyStan)
			{
			case PCB::stanProcesu::Aktywny:
				if ((*iter)->Stan == PCB::stanProcesu::Czekaj¹cy) {
					for (list<PCB*>::iterator it = oczekujaceProcesy.begin(); it != oczekujaceProcesy.end(); /*increment in body*/)
					{
						if ((*it)->PID == ID) //jesli ID sie zgadza usuniêcie wskaŸnika z listy
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
					//for (list<PCB*>::iterator it = Scheduler::ReadyProcessList.begin(); it != Scheduler::ReadyProcessList.end(); /*increment in body*/)
					
					auto it = Scheduler::ReadyProcessList.begin();

					//for (; Scheduler::it != Scheduler::ReadyProcessList.end(); /*increment in body*/){
					for (; it != Scheduler::ReadyProcessList.end(); /*increment in body*/){
						
						if ((*it)->PID == ID) //jesli ID sie zgadza usuniêcie wskaŸnika z listy
						{
							it = Scheduler::ReadyProcessList.erase(it);
							break;
						}
				    	else
						{
							it++;
						}
					}
				}
				/*NA KONIEC MOZNA SPOKOJNIE ZMIENIC STAN*/
				//(*iter)->Stan = PCB::stanProcesu::Aktywny;
				Scheduler::Running->Stan = PCB::stanProcesu::Aktywny;
				break;

			case PCB::stanProcesu::Nowy:
				if ((*iter)->Stan == PCB::stanProcesu::Czekaj¹cy) {
					for (list<PCB*>::iterator it = oczekujaceProcesy.begin(); it != oczekujaceProcesy.end(); /*increment in body*/)
					{
						if ((*it)->PID == ID) //jesli ID sie zgadza usuniêcie wskaŸnika z listy
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
					//for (list<PCB*>::iterator it = Scheduler::ReadyProcessList.begin(); it != Scheduler::ReadyProcessList.end(); /*increment in body*/)
					for (; Scheduler::it != Scheduler::ReadyProcessList.end(); /*increment in body*/)
					{
						PCB *abc = *Scheduler::it;
						if ((abc)->PID == ID) //jesli ID sie zgadza usuniêcie wskaŸnika z listy
						{
							Scheduler::it = Scheduler::ReadyProcessList.erase(Scheduler::it);
							break;
						}
						else
						{
							++Scheduler::it;
						}
					}
				}
				/*NA KONIEC MOZNA SPOKOJNIE ZMIENIC STAN*/
				(*iter)->Stan = PCB::stanProcesu::Nowy;
				break;

			case PCB::stanProcesu::Gotowy:
				if ((*iter)->Stan == PCB::stanProcesu::Czekaj¹cy) {
					for (list<PCB*>::iterator it = oczekujaceProcesy.begin(); it != oczekujaceProcesy.end(); /*increment in body*/)
					{
						if ((*it)->PID == ID) //jesli ID sie zgadza usuniêcie wskaŸnika z listy
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
				// DODAC ZE STANU NOWY
				(*iter)->Stan = PCB::stanProcesu::Gotowy;
				//gotoweProcesy.push_back(*iter);
				Scheduler::ReadyProcessList.push_back(*iter);

				break;

			case PCB::stanProcesu::Czekaj¹cy:	
				if ((*iter)->Stan == PCB::stanProcesu::Gotowy) {
					auto it = Scheduler::ReadyProcessList.begin();

					//for (; Scheduler::it != Scheduler::ReadyProcessList.end(); /*increment in body*/){
					for (; it != Scheduler::ReadyProcessList.end(); /*increment in body*/) {

						if ((*it)->PID == ID) //jesli ID sie zgadza usuniêcie wskaŸnika z listy
						{
							it = Scheduler::ReadyProcessList.erase(it);
							break;
						}
						else
						{
							it++;
						}
					}
					
					//for (list<PCB*>::iterator it = Scheduler::ReadyProcessList.begin(); it != Scheduler::ReadyProcessList.end(); /*increment in body*/
					/*for (; Scheduler::it != Scheduler::ReadyProcessList.end(); ){
						PCB *abc = *Scheduler::it;
						if ((abc)->PID == ID) //jesli ID sie zgadza usuniêcie wskaŸnika z listy
						{
							Scheduler::it = Scheduler::ReadyProcessList.erase(Scheduler::it);
							break;
						}
						else
						{
							++Scheduler::it;
						}
					}*/
				}
				else if ((*iter)->Stan == PCB::stanProcesu::Aktywny) {
					Scheduler::Running->Stan = PCB::stanProcesu::Czekaj¹cy;
					Scheduler::Schedule();
				}
				(*iter)->Stan = PCB::stanProcesu::Czekaj¹cy;
				oczekujaceProcesy.push_back(*iter);
				break;

			case PCB::stanProcesu::Zakoñczony:
				if ((*iter)->Stan == PCB::stanProcesu::Czekaj¹cy) {
					for (list<PCB*>::iterator it = oczekujaceProcesy.begin(); it != oczekujaceProcesy.end(); /*increment in body*/)
					{
						if ((*it)->PID == ID) //jesli ID sie zgadza usuniêcie wskaŸnika z listy
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
					//for (list<PCB*>::iterator it = Scheduler::ReadyProcessList.begin(); it != Scheduler::ReadyProcessList.end(); /*increment in body*/)
					
					auto it = Scheduler::ReadyProcessList.begin();

					//for (; Scheduler::it != Scheduler::ReadyProcessList.end(); /*increment in body*/){
					for (; it != Scheduler::ReadyProcessList.end(); /*increment in body*/) {

						if ((*it)->PID == ID) //jesli ID sie zgadza usuniêcie wskaŸnika z listy
						{
							it = Scheduler::ReadyProcessList.erase(it);
							break;
						}
						else
						{
							it++;
						}
					}
				}
				else if ((*iter)->Stan == PCB::stanProcesu::Aktywny) {
					Scheduler::Schedule();
				}
				(*iter)->Stan = PCB::stanProcesu::Zakoñczony;
				//iter = wszystkieProcesy.erase(iter);
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
		iter != wszystkieProcesy.end(); iter++)
	{
		temp = *iter;
		if (temp->PID == ID)
			return temp;

	}
	return nullptr;

}

void drzewoProcesow()
{
	cout << "ID procesow znajdujacych sie na liscie: ";
	for (list<PCB*>::iterator iter = wszystkieProcesy.begin(); iter != wszystkieProcesy.end(); iter++)
	{
		if ((*iter)->idPotomnych.size() != 0)
		{
			cout << "Rodzic: " << (*iter)->PID << " Procesy potomne: ";
			for (int i : (*iter)->idPotomnych)
			{
				cout << i << " ";
			}
			cout << endl;
		}
	}
	cout << endl;
}

void wypiszListe(list<PCB*> lista)  //ARGUMENTEM MOZE BYC: wszystkieProcesy, gotoweProcesy, oczekujaceProcesy, Scheduler::ReadyProcessList
{
	cout << "ID procesow znajdujacych sie na liscie: ";
	for (list<PCB*>::iterator iter = lista.begin();		iter != lista.end(); iter++)
	{
		cout << (*iter)->PID << ", ";
	}
	cout << endl;
}

PCB* init()
{
	auto INIT = new PCB();
	INIT->PID = 0; INIT->idRodzica = NULL;
	INIT->Stan = PCB::stanProcesu::Gotowy;
	wszystkieProcesy.push_back(INIT);
	//gotoweProcesy.push_back(INIT);
	//  Scheduler::ReadyProcessList.push_back(INIT);    /*NIE DODAJEMY DO LISTY GOTOWYCH BO BY PRZESZKADZALO */
	return INIT;
}

bool kill(int ID)
{
	bool Usuniety = false;
	for (list<PCB*>::iterator iter = wszystkieProcesy.begin(); iter != wszystkieProcesy.end(); iter++)
	{
		if ((*iter)->PID == ID)
		{
			if ((*iter)->PID == 0) {
				cout << endl << "Nie mozna usunac procesu idle. Aby wyjsc z systemu wpisz 'end'" << endl;
			}
			else {
				if ((*iter)->PID == Scheduler::Running->PID) {
					Scheduler::Running = getProcess((*iter)->idRodzica);
				}

				if ((*iter)->base != -1) {
					pam.XF(*iter);
				}

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
					//wskk->Stan = PCB::stanProcesu::Gotowy;
					zmienStan(wskk->PID, PCB::stanProcesu::Gotowy);
					//gotoweProcesy.push_back(wskk);
					//Scheduler::ReadyProcessList.push_back(wskk);  

					poWait.erase((poWait.begin() + indexSygnalu));
				}
				else if (czyCzekaNaNiego)
				{
					poWait.erase((poWait.begin() + indexSygnalu));
				}

				//SPRAWDZAMY CZY USUWANY PROCES MA PROCESY POTOMNE - JEŒLI TAK, ZOSTAN¥ ONE ZAADOPTOWANE PRZEZ PROCES INIT

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
				PCB* fatherWSK = getProcess((*iter)->idRodzica);
				for (int i = 0; i < fatherWSK->idPotomnych.size(); i++)
				{
					if (fatherWSK->idPotomnych.at(i) == ID) {
						fatherWSK->idPotomnych.erase(fatherWSK->idPotomnych.begin() + i);
					}
				}

				zmienStan(ID, PCB::stanProcesu::Zakoñczony);
				
				for (list<PCB*>::iterator it = wszystkieProcesy.begin(); it != wszystkieProcesy.end(); /*increment in body*/)
				{
					if ((*it)->PID == ID) //jesli ID sie zgadza usuniêcie wskaŸnika z listy
					{
						PostOffice::deletePostBox(ID);
						//it = wszystkieProcesy.erase(it);
						break;
					}
					else
					{
						++it;
					}
				}
				
				Usuniety = true;

				break;
			}
		}
	}
	return Usuniety;
}

void wpisz(PCB* a)
{
	pam.XA(a->fileName, a);
}