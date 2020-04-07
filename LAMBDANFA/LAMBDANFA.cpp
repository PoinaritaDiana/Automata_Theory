#include <iostream>
#include <map>
#include <queue>
#include <string>
#include <set>
#include <fstream>
using namespace std;

class LNFA {
	set<int> Q, F;
	set<char> Sigma;
	set<int> q0;
	map<pair<int, char>, set<int> > delta;

public:
	LNFA() { this->q0.insert(0); }
	LNFA(set<int> Q, set<char> Sigma, map<pair<int, char>, set<int> > delta, set<int> q0, set<int> F)
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
	set<int> getInitialState() const { return this->q0; }
	map<pair<int, char>, set<int> > getDelta() const { return this->delta; }

	friend istream& operator >> (istream&, LNFA&);

	bool isFinalState(set<int>);
	set<int> deltaStar(set<int>, string);
	set<int> lambdaInchidere(int);
};

bool LNFA::isFinalState(set <int> s)
{
	for (int i : s)
		for (int j : F)
			if (i == j) return true;
	return false;
}

set<int> LNFA::deltaStar(set<int> s, string w)
{
	int n = w.length();
	set<int> localinchidere;
	set<int> localFinalStates;
	for (int i : s)
	{
		for (int j : lambdaInchidere(i))
			localinchidere.insert(j);
	}
	for (int i : localinchidere)
		for (int j : delta[{i, char(w[0])}])
			localFinalStates.insert(j);
	localinchidere.clear();
	for (int i : localFinalStates)
		for (int j : lambdaInchidere(i))
			localinchidere.insert(j);
	n--;
	if (n == 0)
	{
		return localinchidere;
	}
	int contor = 0;
	localFinalStates.clear();
	while (n)
	{
		for (int i : localinchidere)
		{
			for (int j : delta[{i, w[contor + 1]}])
				localFinalStates.insert(j);
		}
		localinchidere.clear();
		for (int i : localFinalStates)
			for (int j : lambdaInchidere(i))
				localinchidere.insert(j);
		n--;
		contor++;
		localFinalStates.clear();
	}
	return localinchidere;
}

set<int> LNFA::lambdaInchidere(int q)
{
	set<int> inchidere;
	queue <int> coadaInchidere;
	inchidere.insert(q);
	for (int i : delta[{q, char('#')}])
	{
		inchidere.insert(i);
	}
	for (int i : inchidere)
		coadaInchidere.push(i);
	while (coadaInchidere.size() != 0)
	{
		int stare = coadaInchidere.front();
		for (int i : delta[{stare, char('#')}]) {
			if (inchidere.find(i) == inchidere.end())
			{
				inchidere.insert(i);
				coadaInchidere.push(i);
			}
		}
		coadaInchidere.pop();
	}
	return inchidere;
}

istream& operator >> (istream& f, LNFA& M)
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
		int s, d, nr;
		char ch;
		//Lambda = '#'
		f >> s >> ch >> nr;
		for (int j = 0; j < nr; j++)
		{
			f >> d;
			M.delta[{s, ch}].insert(d);
		}
	}
	int x;
	f >> x;
	M.q0.insert(x);

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

int main()
{
	LNFA M;
	ifstream fin("lnfa.txt");
	// exemple cuvinte acceptate: baa, aab, baba
	// exemple respinse: bbbbb
	fin >> M;
	fin.close();
	string cuvant;
	cout << "Introduceti cuvant:";
	cin >> cuvant;
	set<int> lastState = M.deltaStar(M.getInitialState(), cuvant);

	if (M.isFinalState(lastState))
	{
		cout << "Cuvant acceptat";
	}
	else
	{
		cout << "Cuvant respins";
	}
	return 0;
}

