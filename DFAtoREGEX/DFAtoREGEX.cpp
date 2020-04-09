#include <iostream>
#include <map>
#include <string>
#include <set>
#include <fstream>
using namespace std;

const int maxSize = 100;
const string Lambda = "#";

class DFA
{
	set<int> Q, F;
	set<char> Sigma;
	int q0;
	map<pair<int, char>, int> delta;

public:
	DFA() { this->q0 = 0; }
	DFA(set<int> Q, set<char> Sigma, map<pair<int, char>, int> delta, int q0, set<int> F){
		this->Q = Q;
		this->Sigma = Sigma;
		this->delta = delta;
		this->q0 = q0;
		this->F = F;
	}

	set<int> getQ() const { return this->Q; }
	set<int> getF() const { return this->F; }
	set<char> getSigma() const { return this->Sigma; }
	int getInitialState() const { return this->q0; }
	map<pair<int, char>, int> getDelta() const { return this->delta; }

	friend istream& operator >> (istream&, DFA&);
	friend string dfa_to_regex(DFA&);
};

istream& operator >> (istream& f, DFA& M){
	int noOfStates;
	f >> noOfStates;
	for (int i = 0; i < noOfStates; ++i){
		int q;
		f >> q;
		M.Q.insert(q);
	}

	int noOfLetters;
	f >> noOfLetters;
	for (int i = 0; i < noOfLetters; ++i){
		char ch;
		f >> ch;
		M.Sigma.insert(ch);
	}

	int noOfTransitions;
	f >> noOfTransitions;
	for (int i = 0; i < noOfTransitions; ++i){
		int s, d;
		char ch;
		f >> s >> ch >> d;
		M.delta[{s, ch}] = d;
	}

	f >> M.q0;

	int noOfFinalStates;
	f >> noOfFinalStates;
	for (int i = 0; i < noOfFinalStates; ++i){
		int q;
		f >> q;
		M.F.insert(q);
	}

	return f;
}

string dfa_to_regex(DFA& M){
	string regex[maxSize][maxSize] = { "" };			// O matrice care retine expr. regulate (string) dintre oricare doua stari ale automatului

	int nrStari = M.Q.size();							// Nr. de stari pentru AFE este initial egal cu nr. stari AFD
	int okinitial = 1;									// okinitial devine 0 daca există tranzitii care ajung in starea inițială
	int okfinal = 1;									// okfinal devine 0 dacă există tranzitii care pleacă din vreo stare finală

	// In cazul in care este necesar sa introduc o noua stare initiala ( adica okinitial devine 0/există săgeți care ajung către starea inițială a AFD-ului):
	// salvez in matrice tranzitia(qi,qj) drept regex[i+1][j+1] pentru a putea pune noua stare initiala q0 si tranzitiile ei (pastrez linia 0 si coloana 0)
	// Retin in matrice literele cu care s-au realizat tranzitiile dintre doua stari ale AFD

	for (auto t : M.delta) {
		if (regex[t.first.first + 1][t.second + 1] == "")						//Daca nu au mai fost retinute si alte tranzitii anterior
			regex[t.first.first + 1][t.second + 1] = t.first.second;
		else 																	//Daca mai sunt si alte tranzitii, atunci reunesc cu tranzitia curenta
			regex[t.first.first + 1][t.second + 1] = regex[t.first.first + 1][t.second + 1] + "+" + t.first.second;

		if (t.second == M.q0)								// daca exista o tranzitie catre starea initiala a AFD
			okinitial = 0;
		if (M.F.find(t.first.first) != M.F.end())			// dacă există tranzitii care pleacă din vreo stare finală
			okfinal = 0;
	}

	int stareInitiala = M.q0+1;				// La inceput, starea initiala este cea a AFD-ului
	int stareFinala;
	if(M.F.size()==1)
		stareFinala = *M.F.begin();			// La inceput, starea finala este cea a AFD-ului (daca exista o singura stare finala)

	// Daca okinitial==0, inseamna ca trebuie sa adaug la automat o nouă stare care va fi iniţială şi va avea o tranzitie λ către fosta stare iniţială
	if (okinitial == 0) {
		regex[0][M.q0 + 1] = Lambda;
		nrStari++;
		stareInitiala = 0;
	}
	
	// Dacă există mai multe stări finale sau dacă există tranzitii care pleacă din vreo stare finală, 
	// atunci adaug la automat o nouă stare care va fi singura finală şi va avea tranzitii λ din toate fostele stări finale către ea
	if (M.F.size() > 1 || okfinal == 0) {
		for (int i : M.F)
			regex[i + 1][M.Q.size()+1] = Lambda;
		nrStari++;
		stareFinala = M.Q.size()+1;
	}
	
	set <int> stariEliminare;								// Retin starile care trebuie eliminate din automat (toate, mai putin starea initiala si cea finala)
	for (int i : M.Q) {
		if (i == M.q0) {
			if (okinitial == 0)								// Daca am introdus o stare initiala noua, inseamna ca trebuie sa elimin si starea initiala veche
				stariEliminare.insert(i + 1);
		}
		else {
			if (M.F.find(i) != M.F.end()) {
				if (okfinal == 0)							// Daca am introdus o stare finala noua, inseamna ca trebuie sa elimin si starile finale vechi
					stariEliminare.insert(i + 1);
			}
			else
				stariEliminare.insert(i + 1);				// Daca nu e nici stare initiala, nici finala, trebuie eliminata oricum
		}
	}

	set <int> stariCurente;									// Retin in "stariCurente" starile care au mai ramas dupa fiecare eliminare in automat
	for (int i = 0; i < nrStari; i++)						// Initial, sunt toate starile automatului
		stariCurente.insert(i);


	//Elimin pe rand fiecare stare din stariEliminare
	// "*"=stelare; "+"=reuniune; altfel, concatenare

	for (int stare : stariEliminare) {
																		// Verific daca starea are exista tranzitia (qi,qi)
		string bucla = "";
		if (regex[stare][stare] != ""){									// Daca nu e vid, inseamna ca are bucla
			if (regex[stare][stare].length() == 1)  
				bucla = regex[stare][stare] + "*";						// Daca e un singur caracter nu are rost sa pun paranteze 
				bucla = "(" + regex[stare][stare] + ")" + "*";
		}
		
		stariCurente.erase(stare);					// Sterg din set starea pe care urmeaza sa o elimin (pentru ca nu o sa mai fie in automat)


		set <int> stariIn;							// Retin starile qi pentru care exista tranzitie (qi, stare_de_eliminat) / "intra" in stare_de_eliminat - ma uit pe coloana stare
		set <int> stariOut;							// Retin starile qj pentru care exista tranzitie (stare_de_eliminat, qj) / "ies" din stare_de_eliminat - ma uit pe linia stare
		for (int i: stariCurente) {
			if (regex[i][stare] != "")
				stariIn.insert(i);
			if (regex[stare][i] != "")
				stariOut.insert(i);
		}

		// Le refac astfel: regex[i][j] = regex[i][j] + (regex[i][stare]) x (bucla) x (regex[stare][j]);
		// Daca elimin o stare, trebuie sa refac toate tranzitiile dintre starile care "intra" (stariIn) in starea de eliminat si cele care "ies" (stariOut) - produs cartezian

		//Am pus niste if-uri ca sa mai scap de niste paranteze inutile, dar chiar nu stiu cum as putea sa le explic 
		for(int i: stariIn)
			for (int j : stariOut) {
				if (regex[i][j] != "") {
					regex[i][j] = "(" + regex[i][j] + "+";
					if (regex[i][stare] == Lambda) {
						if (bucla == "" && regex[stare][j] == Lambda)  regex[i][j] = regex[i][j] + regex[i][stare];
						else {
							if (bucla != "") {
								regex[i][j] = regex[i][j] + bucla;
								if (regex[stare][j] != Lambda)  regex[i][j] = regex[i][j] + regex[stare][j];
							}
							else regex[i][j] =  regex[i][j] + regex[stare][j];	
						}
					}
					else {
						regex[i][j] = regex[i][j] + regex[i][stare];
						if (bucla != "")  regex[i][j] = regex[i][j] + bucla;
						if (regex[stare][j] != Lambda)  regex[i][j] = regex[i][j] + regex[stare][j];
					}
					regex[i][j] = regex[i][j] + ")";
				}
				else {
					if (regex[i][stare] == Lambda) {
						if (bucla == "" && regex[stare][j] == Lambda)  regex[i][j] = regex[i][stare];
						else {
							if (bucla != "") {
								regex[i][j] = bucla;
								if (regex[stare][j] != Lambda)  regex[i][j] = regex[i][j] + regex[stare][j];
							}
							else regex[i][j] = regex[stare][j];
							
						}
					}
					else {
						regex[i][j] = regex[i][stare];
						if (bucla != "")  regex[i][j] = regex[i][j] + bucla;
						if(regex[stare][j]!=Lambda)  regex[i][j] = regex[i][j] + regex[stare][j];
					}
				}
					
			}	
	}

	return regex[stareInitiala][stareFinala];			// Returnez expresia regulata dintre starea initiala si cea finala
}

int main()
{
	DFA M;
	ifstream fin("dfa.txt");
	fin >> M;
	fin.close();
	string expresie = dfa_to_regex(M);
	cout << expresie;
	return 0;
}
