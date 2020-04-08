#include <iostream>
#include <map>
#include <string>
#include <set>
#include <fstream>
using namespace std;

const int maxSize = 100;
const string Lambda = "@";

class DFA
{
	set<int> Q, F;
	set<char> Sigma;
	int q0;
	map<pair<int, char>, int> delta;

public:
	DFA() { this->q0 = 0; }
	DFA(set<int> Q, set<char> Sigma, map<pair<int, char>, int> delta, int q0, set<int> F)
	{
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

istream& operator >> (istream& f, DFA& M)
{
	int noOfStates;
	f >> noOfStates;
	for (int i = 0; i < noOfStates; ++i)
	{
		int q;
		f >> q;
		M.Q.insert(q);
	}

	int noOfLetters;
	f >> noOfLetters;
	for (int i = 0; i < noOfLetters; ++i)
	{
		char ch;
		f >> ch;
		M.Sigma.insert(ch);
	}

	int noOfTransitions;
	f >> noOfTransitions;
	for (int i = 0; i < noOfTransitions; ++i)
	{
		int s, d;
		char ch;
		f >> s >> ch >> d;
		M.delta[{s, ch}] = d;
	}

	f >> M.q0;

	int noOfFinalStates;
	f >> noOfFinalStates;
	for (int i = 0; i < noOfFinalStates; ++i)
	{
		int q;
		f >> q;
		M.F.insert(q);
	}

	return f;
}

string dfa_to_regex(DFA& M){
	string regex[maxSize][maxSize] = { "" }; // O matrice care retine expresile regulate dintre starile automatului

	int nrStari = M.Q.size(); //Nr. de stari pentru AFE este initial egal cu nr. stari AFD
	int okinitial = 1; // okinitial devine 0 daca există săgeți care ajung către starea inițială
	int okfinal = 1; //okfinal devine 0 dacă există săgeți care pleacă din vreo stare finală
	//In cazul in care este necesar sa introduc o noua stare initiala: stare++, pentru a putea pune noua stare initiala q0
	//Marchez in matrice literele cu care s-au realizat tranzitiile dintre doua stari
	for (auto t : M.delta) {
		if (regex[t.first.first + 1][t.second + 1] == "") //Daca nu mai sunt si alte tranzitii
			regex[t.first.first + 1][t.second + 1] = t.first.second;
		else
			regex[t.first.first + 1][t.second + 1] = regex[t.first.first + 1][t.second + 1] + "+" + t.first.second;
			//Daca mai sunt si alte tranzitii, atunci reunim cu noua tranzitie 

		if (t.second == M.q0) //daca e stare initiala
			okinitial = 0;
		if (M.F.find(t.first.first) != M.F.end()) // daca e stare finala
			okfinal = 0;
	}

	int stareInitiala = M.q0+1;
	int stareFinala = -1;
	if(M.F.size()==1)
		stareFinala = *M.F.begin();

	// Daca okinitial==0, inseamna ca trebuie sa adaug la automat o nouă stare care va fi iniţială şi va avea o săgeată cu expresia λ către fosta stare iniţială
	if (okinitial == 0) {
		regex[0][M.q0 + 1] = Lambda;
		nrStari++;
		stareInitiala = 0;
	}
	
	/*Dacă există mai multe stări finale sau dacă există săgeți care pleacă din vreo stare finală, 
	  atunci se adaugă la automat o nouă stare care va fi singura finală şi va avea săgeți cu expresia λ din toate fostele stări finale către ea. */
	if (M.F.size() > 1 || okfinal == 0) {
		for (int i : M.F)
			regex[i + 1][M.Q.size()+1] = Lambda;
		nrStari++;
		stareFinala = M.Q.size()+1;
	}
	
	set <int> stariEliminare;  //Retin starile care trebuie eliminate
	for (int i : M.Q) {
		if (i == M.q0) {
			if (okinitial == 0) //daca am introdus o stare initiala noua, inseamna ca trebuie sa elimin si starea initiala veche
				stariEliminare.insert(i + 1);
		}
		else {
			if (M.F.find(i) != M.F.end()) {
				if (okfinal == 0)  //daca am introdus o stare finala noua, inseamna ca trebuie sa elimin si starile finale vechi
					stariEliminare.insert(i + 1);
			}
			else
				stariEliminare.insert(i + 1); //daca nu e nici stare initiala, nici finala, trebuie eliminata oricum
		}
	}

	set <int> stariCurente;
	for (int i = 0; i < nrStari; i++)
		stariCurente.insert(i);

	//Eliminam pe rand fiecare stare din stariEliminare
	for (int stare : stariEliminare) {
		//Verific mai intai daca starea are bucla sau nu
		string bucla = "";
		if (regex[stare][stare] != "") //Inseamna ca are bucla
			bucla = "(" + regex[stare][stare] + ")" + "*";
		
		set <int> stariIn; //Starile qi pentru care exista tranzitie (qi,starea curenta)  - ma uit pe coloana stare
		set <int> stariOut; //Starile qj pentru care exista tranzitie (starea curenta, qj)  - ma uit pe linia stare

		for (int i: stariCurente) {
			if (i != stare) {
				if (regex[i][stare] != "")
					stariIn.insert(i);
				if (regex[stare][i] != "")
					stariOut.insert(i);
			}
		}

		stariCurente.erase(stare); //Stergem din vector starea pe care tocmai am eliminat o

		for(int i: stariIn)
			for (int j : stariOut) {
				if (regex[i][j] != "") {
					if(regex[i][j]!=Lambda)
						regex[i][j] = "(" + regex[i][j] + ")";
					if (regex[i][stare] != "")
						regex[i][j] = regex[i][j] + "+" + "(" + regex[i][stare] + ")";
					if (bucla != "")
						regex[i][j] = regex[i][j] + "(" + bucla + ")";
					if(regex[stare][j]!="")
						regex[i][j] = regex[i][j] + "(" + regex[stare][j] + ")";
				}
				else {
					if (regex[i][stare] != "")
						regex[i][j] = "(" + regex[i][stare] + ")";
					if (bucla != "")
						regex[i][j] = regex[i][j] + "(" + bucla + ")";
					if (regex[stare][j] != "")
						regex[i][j] = regex[i][j] + "(" + regex[stare][j] + ")";
				}
			}


				//regex[i][j] = "(" + regex[i][j] + ")" + "+" + "(" + regex[i][stare] + ")" + "(" + bucla + ")" + "(" + regex[stare][j] + ")";

		stariIn.clear();
		stariOut.clear();
			
	}
	
	return regex[stareInitiala][stareFinala];

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
