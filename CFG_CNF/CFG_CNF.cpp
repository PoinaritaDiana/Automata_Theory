#include <iostream>
#include <set>
#include <string>
#include <map>
#include <vector>]
using namespace std;

map <string, set<string>> P;	//Productii
set <string> N;					//Neterminale
set <char> Sigma;				//Terminale
string S;
char caracter;

void configGrammar() {
	P["S"].insert("aAb"); P["S"].insert("BC"); P["S"].insert("@");
	P["A"].insert("AaB"); P["A"].insert("Db");
	P["B"].insert("b"); P["B"].insert("C");
	P["C"].insert("@"); P["C"].insert("ab");
	P["D"].insert("@");
	P["F"].insert("a");

	N.insert("S"); N.insert("A"); N.insert("B");
	N.insert("C"); N.insert("D"); N.insert("F");
	Sigma.insert('a'); Sigma.insert('b');
	S = "S";

	caracter = 'F';
}

void eliminateStart() {
	N.insert("S0");
	S = "S0";
	P["S0"].insert("S");
}

void lambdaProductions() {
	set <string> eliminare;
	//Pentru fiecare neterminal
	for (auto n : N) {
		set<string>::iterator it;
		it = P[n].find("@");
		//Daca se gaseste lambda
		if (it != P[n].end()) {
			if (P[n].size() == 1) eliminare.insert(n);
		}
	}
}

void unitProductions() {

}

void uselessProductions() {

}

void eliminateTerminals() {
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


void eliminateMoreNonTerminals() {
	//Neterminalele noi adaugate
	set <char> newAdd;
	int ok = 0;
	for (auto n : N) {
		set<string> Pnou;	//Retin toate productiile (cu modificari, daca e cazul)
		for (auto p : P[n]) {
			if (p.size() >= 3) {
				ok = 1;
				//Neterminalul de inserat
				caracter++;
				string sc = string(1, caracter);
				string rest = p.substr(1);
				p = p[0] + sc;
				P[sc].insert(rest);
				newAdd.insert(caracter);
			}
			Pnou.insert(p);
		}
		P[n] = Pnou;
	}

	//Adaug neterminalele noi
	if (newAdd.size() != 0) {
		for (auto s : newAdd)
			N.insert(string(1,s));
	}

	if(ok==1)
		eliminateMoreNonTerminals();
}


int main() {
	configGrammar();
	eliminateTerminals();
	eliminateMoreNonTerminals();

	for (auto p : P) {
		cout << p.first << "->";
		for (auto e : p.second)cout << e << ",";
		cout << endl;
	}

	return 0;
}