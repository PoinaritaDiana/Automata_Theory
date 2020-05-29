#include <iostream>
#include <set>
#include <string>
#include <map>
#include <vector>
using namespace std;

map <string, set<string>> P;	//Productii
set <string> N;					//Neterminale
set <char> Sigma;				//Terminale
char caracter;					//Nou neterminal 

//Configurare gramatica independenta de context
void configGrammar() {
	P["S"].insert("aBcDeF"); P["S"].insert("HF"); P["S"].insert("HBc");
	P["B"].insert("b"); P["B"].insert("@");
	P["D"].insert("d"); P["D"].insert("@");
	P["F"].insert("G");
	P["G"].insert("f"); P["G"].insert("g");
	P["H"].insert("@");

	N.insert("S"); N.insert("D"); N.insert("B");
	N.insert("G"); N.insert("F"); N.insert("H");
	Sigma.insert('a'); Sigma.insert('b');
	Sigma.insert('c'); Sigma.insert('d');
	Sigma.insert('e'); Sigma.insert('f');
	Sigma.insert('g');

	caracter = 'H';
}

//Algoritm de reducere
void uselessProductions() {
	//Simboluri din N care sunt utilizabile
	/*Se pleaca de la cele care au cel putin o productie cu membrul drept neterminal
		Tot caut alte neterminale care au cel putin o productie cu membrul drept in (N U T)*
		dar in care toate neterminalele fac deja parte din multimea celor utilizabile*/

	set <string> n1;
	for (auto n : N)
		for (auto p : P[n]) {
			int i = 0;
			while (i < p.size() && N.find(string(1, p[i])) == N.end())
				i++;
			if (i == p.size()) {
				n1.insert(n);
				break;
			}
		}
	int ok;
	do {
		ok = 0;
		for (auto n : N)
			if (n1.find(n) == n1.end()) {
				for (auto p : P[n]) {
					int i = 0;
					while (i < p.size() && (N.find(string(1, p[i])) == N.end() || n1.find(string(1, p[i])) != n1.end()))
						i++;
					if (i == p.size()) {
						n1.insert(n);
						ok = 1;
						break;
					}
				}
			}
	} while (ok);

	//Simboluri din N care sunt accesibile
	/*Plec de la neterminalele care apar in membrul drept al productiilor lui S
		Si tot caut alte neterminale care apar in membrul drept al productiilor acelor neterminale
		care fac deja parte din multimea celor accesibile*/
	vector <string> N2;
	set <string> n2;
	n2.insert("S");
	N2.push_back("S");
	int i = 0;
	while (i < N2.size()) {
		string n = N2[i];
		for (auto p : P[n]) {
			for (int j = 0; j < p.size(); j++)
				if (N.find(string(1, p[j])) != N.end() && n2.find(string(1, p[j])) == n2.end()) {
					n2.insert(string(1, p[j]));
					N2.push_back(string(1, p[j]));
				}
		}
		i++;
	}

	//Elementele comune = ce trebuie sa ramana in N
	set<string>::iterator it = n1.begin();
	while (it != n1.end()) {
		if (n2.find(*it) == n2.end()) {
			N.erase(*it);
			P.erase(*it);
			it=n1.erase(it);
		}
		else{
			n2.erase(*it);
			it++;
		}
	}
	for (auto s : n2) {
		N.erase(s);
		P.erase(s);
	}
		
	//Sterg productiile care contin neterminale care nu sunt in N
	for (auto n : N) {
		set<string> Pnou;
		for (auto p : P[n]) {
			int i = 0;
			while (i < p.size()) {
				if (p[i] != '@' && Sigma.find(p[i]) == Sigma.end() && N.find(string(1, p[i])) == N.end())
					break;
				i++;
			}
			if (i == p.size())
				Pnou.insert(p);
		}
		P[n] = Pnou;
	}
}

//Eliminarea λ-producțiilor
void lambdaProductions() {

	/*Daca neterminalul care are o λ–productie nu mai are si alte productii, atunci
		- va fi eliminata productia lui
		- toate productiile care au ca membru drept un cuvant de lungime minim 2 in care apare
		acest neterminal vor fi inlocuite prin eliminarea neterminalului din cuvinte
		- daca membrul drept avea lungime 1 (adica era doar acest neterminal), atunci se
		înlocuieste cu @ (lambda) (dar si aceasta producţie va fi ulterior eliminată)

	Dacă neterminalul care are o λ–producţie are şi alte producţii, atunci
		- va fi eliminată doar λ–producţia lui
		- toate productiile care au ca membru drept un cuvant de lungime minim 2 in care apare
		acest neterminal vor fi inlocuite atât de varianta în care cuvântul conţine
		neterminalul, cât şi de varianta în care neterminalul este eliminat din cuvânt
		- dacă membrul drept avea lungime 1 (adică era doar acest neterminal), atunci această
		producţie este o redenumire şi va fi eliminată ulterior*/

	set <string> eliminare;
	int ok = 0;
	//Pentru fiecare neterminal
	for (auto n : N) {
		if (P[n].find("@") != P[n].end()) {
			ok = 1;
			if (P[n].size() == 1) {
				eliminare.insert(n);
				for (auto nn : N)
					if (nn != n) {
						set<string> Pnou;
						for (auto p : P[nn]) {
							if (p.size() == 1 && p == n) {
								p = "@";
							}
							if (p.size() > 1 && p.find(n) != string::npos) {
								int poz = p.find(n);
								p.erase(poz, 1);
							}
							Pnou.insert(p);
						}
						P[nn] = Pnou;
					}
			}
			else {
				P[n].erase("@");
				for (auto nn : N){
					set<string> Pnou;
					string vechip = "";
					for (auto p : P[nn]) {
						if (p.size() > 1 && p.find(n) != string::npos) {
							vechip = p;
							int poz = p.find(n);
							p.erase(poz, 1);
						}
						Pnou.insert(p);
						if (vechip != "") {
							Pnou.insert(vechip);
							vechip = "";
						}
					}
					P[nn] = Pnou;
				}
			}
		}
	}
	for (auto s : eliminare) {
		N.erase(s);
		P.erase(s);
	}
	if (ok == 1)
		lambdaProductions();
}

//Eliminarea redenumirilor
void unitProductions() {

	/*In gramatică vrem sa avem numai producţii care au ca membru drept un neterminal.
	Înlocuiesc neterminalul din dreapta cu toate cuvintele care sunt membru drept în producţiile sale.
	Verific dacă acest neterminal din dreapta mai apare în dreapta în cadrul altor producţii:
	- Daca nu, atunci elimin toate producţiile pe care le avea
	- Dacă mai apare în cuvinte de lungime minim 2, atunci trebuie să le păstrez
	Dacă apare în cuvânt de lungime 1, înseamnă că este tot o redenumire şi va fi eliminată.*/


	int ok;
	do {
		ok = 0;
		for (auto n : N) {
			set<string> Pnou;
			for (auto p : P[n]) {
				if (p != n && N.find(p) != N.end()) {
					ok = 1;
					for (auto pp : P[p])
						Pnou.insert(pp);
				}
				else
					Pnou.insert(p);
			}
			P[n] = Pnou;
		}
	} while (ok);
}

//Adăugare neterminale noi pentru terminalele din cuvinte
void eliminateTerminals() {
	/*Terminalele trebuie să apară doar singure în membrul drept.
		De aceea, peste tot unde apar în componenţa unui cuvânt de lungime minim 2, 
		le înlocuiesc cu un neterminal nou şi adaug producţia de la neterminalul nou
		la terminalul pe care l-a înlocuit.*/

	//Productiile noi adaugate
	set <pair<char, char>> newAdd;

	for (auto n : N) {
		set<string> Pnou;	//Retin toate productiile (cu modificari, daca e cazul)
		for (auto p : P[n]) {
			if (p.size() >= 2) {
				for (int i = 0; i < p.size(); i++) {
					if (Sigma.find(p[i]) != Sigma.end()) {
						int ok = 0;
						char caracternou;
						for (auto s : newAdd)
							if (s.second == p[i]) {
								caracternou = s.first;
								ok = 1;
								break;
							}
						if (ok == 1)
							p[i] = caracternou;
						else {
							caracter++;
							if (caracter == 'S')
								caracter++;
							string sc = string(1, caracter);
							P[sc].insert(string(1, p[i]));
							newAdd.insert({ caracter,p[i] });
							p[i] = caracter;
						}
					}
				}
			}
			Pnou.insert(p);
		}
		P[n] = Pnou;
	}

	//Adaug neterminalele noi
	if (newAdd.size() != 0) {
		for (auto s : newAdd)
			N.insert(string(1, s.first));
	}
}

//Adăugare neterminale noi pentru „spargerea” cuvintelor lungi (>2)
void eliminateMoreNonTerminals() {

	/*In dreapta trebuie să fie cuvinte formate din exact două neterminale.
		De aceea, unde sunt cuvinte mai lungi, păstrez doar primul neterminal din cuvânt
		şi îi alipesc un neterminal nou,
		iar noul neterminal va avea o producţie cu membrul drept cuvântul pe care l-a înlocuit.
		Procedeul se reia până când toate cuvintele ajung la lungimea 2.*/

	//Neterminalele noi adaugate
	set <pair<char,string>>newAdd;
	int ok=0;
	do {
		ok = 0;
		for (auto n : N) {
			set<string> Pnou;	//Retin toate productiile (cu modificari, daca e cazul)
			for (auto p : P[n]) {
				if (p.size() >= 3) {
					ok = 1;
					string rest = p.substr(1);

					int exista = 0;
					char caracternou;
					for (auto s : newAdd)
						if (s.second == rest) {
							exista = 1;
							caracternou = s.first;
							break;
						}
					if (exista == 1)
						p = p[0] + string(1, caracternou);
					else {
						//Neterminalul de inserat
						caracter++;
						if (caracter == 'S')
							caracter++;
						string sc = string(1, caracter);
						p = p[0] + sc;
						P[sc].insert(rest);
						newAdd.insert({ caracter,rest });
					}
				}
				Pnou.insert(p);
			}
			P[n] = Pnou;
		}
		//Adaug neterminalele noi
		if (newAdd.size() != 0) {
			for (auto s : newAdd)
				N.insert(string(1, s.first));
		}
	} while (ok == 1);
}


int main() {
	configGrammar();

	//Am urmarit algoritmul prezentat la curs/seminar
	//Pasul 1 - algoritm de reducere
	uselessProductions();
	//Pasul 2 - Eliminarea λ-producțiilor
	lambdaProductions();
	//Pasul 3 - Eliminarea redenumirilor
	unitProductions();
	//Pasul 4- Se aplică din nou algoritmul de reducere
	uselessProductions();
	//Pasul 5 - Adăugare neterminale noi pentru terminalele din cuvinte
	eliminateTerminals();
	//Pasul 6 - Adăugare neterminale noi pentru „spargerea” cuvintelor lungi (>2)
	eliminateMoreNonTerminals();

	for (auto p : P) {
		cout << p.first << " -> ";
		for (auto e : p.second)
			cout << e << ",";
		cout << endl;
	}

	return 0;
}