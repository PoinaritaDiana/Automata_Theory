#include <iostream>
#include <map>
#include <string>
#include <set>
#include <fstream>
using namespace std;

class NFA
{
	set<int> Q, F;
	set<char> Sigma;
	set <int> q0;
	map<pair<int, char>, set<int> > delta;

public:
	NFA() { this->q0.insert(0); }
	NFA(set<int> Q, set<char> Sigma, map<pair<int, char>, set<int> > delta, set <int> q0, set<int> F)
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
	set <int> getInitialState() const { return this->q0; }
	map<pair<int, char>, set<int> > getDelta() const { return this->delta; }

	friend istream& operator >> (istream&, NFA&);

	bool isFinalState(set<int>);
	set<int> deltaStar(set<int>, string);
};

bool NFA::isFinalState(set <int> s)
{
	for (set<int>::iterator it = s.begin(); it != s.end(); it++)
		for (set<int>::iterator itt = F.begin(); itt != F.end(); itt++)
			if (*it == *itt) return true;
	return false;

}
set<int> NFA::deltaStar(set<int> s, string w)
{
	int n = w.length();
	set <int> localFinalStates;
	for (set<int>::iterator it = delta[{*s.begin(), char(w[0])}].begin(); it != delta[{*s.begin(), char(w[0])}].end(); it++)
	{
		localFinalStates.insert(*it);
	}
	n--;
	if (n == 0)
	{
		return localFinalStates;
	}
	int contor = 0;
	while (n)
	{
		set <int> auxiliar;
		for (set<int>::iterator it = localFinalStates.begin(); it != localFinalStates.end(); it++)
		{
			for (set<int>::iterator itt = delta[{*it, w[contor + 1]}].begin(); itt != delta[{*it, w[contor + 1]}].end(); itt++)
				auxiliar.insert(*itt);
		}
		n--;
		contor++;
		localFinalStates.clear();
		for (set<int>::iterator it = auxiliar.begin(); it != auxiliar.end(); it++)
			localFinalStates.insert(*it);
		auxiliar.clear();
	}
	return localFinalStates;

}

istream& operator >> (istream& f, NFA& M)
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
	NFA M;
	ifstream fin("nfa.txt");
	fin >> M;
	fin.close();

	set<int> lastState = M.deltaStar(M.getInitialState(), "bbaab");

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
