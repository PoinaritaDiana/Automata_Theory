#include <iostream>
#include <set>
#include <string>
#include <map>
#include <vector>
using namespace std;

map <string, set<string>> P;	//Productii
set <string> N;					//Neterminale
set <char> Sigma;				//Terminale
char caracter;

void configGrammar() {
	P["S"].insert("aAbC"); P["S"].insert("dB"); P["S"].insert("@");
	P["A"].insert("bA"); P["A"].insert("@");
	P["B"].insert("d"); P["B"].insert("c");
	P["C"].insert("@"); 
	P["F"].insert("@"); P["F"].insert("A"); P["F"].insert("BC");

	N.insert("S"); N.insert("A"); N.insert("B");
	N.insert("C"); N.insert("F");
	Sigma.insert('a'); Sigma.insert('b');
	Sigma.insert('c'); Sigma.insert('d');
	
	caracter = 'F';
}

void uselessProductions() {
	//Simboluri din N care sunt utilizabile
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

	//Elementele comune = ce trebuie sa ramana (in n1)
	auto it = n1.begin();
	while (it != n1.end()) {
		if (n2.find(*it) == n2.end()) {
			N.erase(*it);
			P.erase(*it);
			it = n1.erase(it);

		}
		else
			it++;
	}

	//Stergem productiile care contine neterminale care nu sunt in N
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

void lambdaProductions() {
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
				for (auto nn : N)
					if (nn != n) {
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

void eliminateStart() {
	N.insert("S0");
	P["S0"].insert("S");
}

void unitProductions() {

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
	
	uselessProductions();
	for (auto p : P) {
		cout << p.first << "->";
		for (auto e : p.second)cout << e << ",";
		cout << endl;
	}
	cout << endl;

	lambdaProductions();
	for (auto p : P) {
		cout << p.first << "->";
		for (auto e : p.second)cout << e << ",";
		cout << endl;
	}
	cout << endl;

	eliminateTerminals();
	for (auto p : P) {
		cout << p.first << "->";
		for (auto e : p.second)cout << e << ",";
		cout << endl;
	}
	cout << endl;

	eliminateMoreNonTerminals();
	for (auto p : P) {
		cout << p.first << "->";
		for (auto e : p.second)cout << e << ",";
		cout << endl;
	}

	return 0;
}