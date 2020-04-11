#include <iostream>
#include <map>
#include <string>
#include <set>
#include <fstream>
#include <vector>
using namespace std;

class DFA{
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
	int getNrStates() const { return this->Q.size();}
	int getInitialState() const { return this->q0; }
	map<pair<int, char>, int> getDelta() const { return this->delta; }

	friend istream& operator >> (istream&, DFA&);
	friend ostream& operator << (ostream&, DFA&);

	//Retin intr-un vector pentru fiecare stare numarul partitie din care face parte la final
	friend vector<int> partition(vector<int>, DFA&);
	friend DFA minimizing_dfa (DFA&);
};

istream& operator >> (istream& f, DFA& M)
{
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

ostream& operator << (ostream& out, DFA& M){
	out << "States of DFA: ";
	for (int i : M.Q) out << i << " ";
	out << endl;
	
	out << "Letters:";
	for (char s : M.Sigma) out << s << " ";
	out << endl;

	out << "Transitions: " << endl;
	for (auto i : M.delta)
		out << "transition (" << i.first.first << "," << i.first.second << ") = " << i.second << endl;

	out << "Start state: ";
	out << M.q0;
	out << endl;

	out << "Number of Final states: ";
	int nr = 0;
	for (int i : M.F) nr++;
	out << nr << endl;
	out << "Final States: ";
	for (int i : M.F)
		out << i << " ";

	return out;
}

vector<int> partition(vector <int> v, DFA& M) {
	
	int verif;
	do {
		vector <int> aux(v.size(), 0);				// Un vector auxiliar in care voi retine partitiile curente
		int nr_part = 1;							// Nr.partitii
		aux[0] = nr_part;							// Starea initiala va avea mereu partitia 1
		for (int i = 1; i < v.size(); i++) {
			int ok = 0;
			int j = 0;								// Pentru fiecare stare qi verific "echivalenta" cu starile qj pentru care deja am stabilit partitia (unde j<i)
			while (ok == 0 && j < i) {
				if (v[i] == v[j]) {					// Daca se afla in aceeasi partitie, continui cu verificarea echivalentei dintre tranzitii; daca nu, trec la urmatoarea stare
					int egal = 1;					// Presupun initial ca cele doua stari sunt "echivalente", adica duc catre aceeasi partitie

					// Daca tranzițiile de la cele două stări qi și qj, cu o aceeași literă, duc spre stări aflate deja la pasul anterior în partiții diferite: sunt separabile
					for (char s : M.getSigma())
						if (v[M.delta[{i, s}]] != v[M.delta[{j, s}]]) egal = 0;

					// Daca cele doua stari sunt "echivalente", inseamna ca qi se afla in aceeasi partitie ca si qj
					if (egal == 1) {
						ok = 1;
						aux[i] = aux[j];
					}
					//Altfel, fac o noua partitie pentru qi
					else {
						nr_part++;
						aux[i] = nr_part;
					}
				}
				j++;
			}
			// Daca nu am gasit nicio stare cu care sa se poata potrivi, inseamna ca trebuie sa pun starea curenta intr-o noua partitie
			if (aux[i] == 0) {
				nr_part++;
				aux[i] = nr_part;
			}
		}

		// Daca nu am facut nicio modificare fata de pasul anterior, inseamna ca nu mai sunt de facut alte partitii si pot returna vectorul
		// Daca exista modificari, inseamna ca trebuie sa mai testez daca mai exista partitii de facut
		verif = 1;
		int i = 0;
		while (verif == 1 && i < v.size()) {
			if (v[i] != aux[i])
				verif = 0;
			i++;
		}
		v = aux;
	}while(verif==0);
	return v;
	
	/*
	vector <int> aux(v.size(), 0);				// Un vector auxiliar in care voi retine partitiile curente
	int nr_part = 1;							// Nr.partitii
	aux[0] = nr_part;							// Starea initiala va avea mereu partitia 1
	for (int i = 1; i < v.size(); i++){
		int ok = 0;								
		int j = 0;								// Pentru fiecare stare qi verific "echivalenta" cu starile qj pentru care deja am stabilit partitia (unde j<i)
		while (ok == 0 && j < i) {
			if (v[i] == v[j]) {					// Daca se afla in aceeasi partitie, continui cu verificarea echivalentei dintre tranzitii; daca nu, trec la urmatoarea stare
				int egal = 1;					// Presupun initial ca cele doua stari sunt "echivalente", adica duc catre aceeasi partitie

				// Daca tranzițiile de la cele două stări qi și qj, cu o aceeași literă, duc spre stări aflate deja la pasul anterior în partiții diferite: sunt separabile
				for (char s : M.getSigma())
					if (v[M.delta[{i, s}]] != v[M.delta[{j, s}]]) egal = 0;	

				// Daca cele doua stari sunt "echivalente", inseamna ca qi se afla in aceeasi partitie ca si qj
				if (egal==1) {
					ok = 1;
					aux[i] = aux[j];
				}
				//Altfel, fac o noua partitie pentru qi
				else {
					nr_part++;
					aux[i] = nr_part;
				}
			}
			j++;
		}
		// Daca nu am gasit nicio stare cu care sa se poata potrivi, inseamna ca trebuie sa pun starea curenta intr-o noua partitie
		if(aux[i]==0){
			nr_part++;
			aux[i] = nr_part;
		}
	}

	// Daca nu am facut nicio modificare fata de pasul anterior, inseamna ca nu mai sunt de facut alte partitii si pot returna vectorul
	// Daca exista modificari, inseamna ca trebuie sa mai testez daca mai exista partitii de facut
	int verif = 1;
	int i = 0;
	while (verif == 1 && i < v.size()) {
		if (v[i] != aux[i])
			verif = 0;
		i++;
	}
	if (verif == 0) 
		return partition(aux, M);
	else
		return aux;
	*/
		
}

DFA minimizing_dfa (DFA& M) {
	vector <int> v(M.getNrStates(), 1);		//Pentru fiecare stare, retin numarul partitiei corespunzatoare
	for (int i : M.F) v[i] = 2;				// Daca este stare finala, se va afla in partitia nr.2; daca nu, in partitia nr.1

	v = partition(v, M);	// Actualizez v in urma partitiilor facute 

	DFA A;					// Automatul minimizat
	A.Sigma = M.Sigma;		//Alfabetul Sigma ramane acelasi

	for (int i = 0; i < v.size(); i++) {
		if (i == M.q0) A.q0 = v[i] - 1;								// Daca qi era stare initiala in AFD-ul initial, atunci si partitia din care face parte e stare initiala
		if (M.F.find(i) != M.F.end()) A.F.insert(v[i] - 1);			// Daca qi era stare finala in AFD-ul initial, atunci si partitia din care face parte e stare finala
		if (A.Q.find(v[i] - 1) == A.Q.end()){
			A.Q.insert(v[i] - 1);									// Fiecare partitie reprezinta, de fapt, o stare in AFD-ul minimizat ( v[i]-1 pentru ca am inceput partitiile de la 1)
			for (char s : A.Sigma)									// Refac tranzitiile din AFD-ul initial
				A.delta[{v[i] - 1, s}] = v[M.delta[{i, s}]]-1;      // Tranzitia (qi,qj) va fi delta[{partitia in care se afla qi, s}] = paritita in care se afla vechiul M.delta[{i, s}]
		}
	}
	return A;    // Returnez AFD-ul modificat
}

int main()
{
	ifstream fin("dfa.txt");
	DFA M;
	fin >> M;
	fin.close();

	DFA N = minimizing_dfa(M);
	cout << N;

	return 0;
}
