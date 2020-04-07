#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <tuple>

const int ErrorState = -1;
const char Lambda = '.';
const char EmptyChar = 0;

#define DeltaType map<pair<pair<int, char>, char>, pair<int, string>>

using namespace std;

class DPDA
{
	set<int> Q, F;
	set<char> Sigma, Gamma;
	int q0;
	char Z;
	DeltaType delta;
	stack<char> Stack;

public:
	DPDA()
	{
		this->q0 = 0;
		this->Z = 'Z';
		Stack.push(this->Z);
	}

	DPDA(set<int> Q, set<char> Sigma, int q0, set<int> F, set<char> Gamma, char Z, DeltaType delta)
	{
		this->Q = Q;
		this->Sigma = Sigma;
		this->q0 = q0;
		this->F = F;
		this->Gamma = Gamma;
		this->Z = Z;
		this->delta = delta;
		Stack.push(Z);
	}

	set<int> getQ() const { return this->Q; }
	set<char> getSigma() const { return this->Sigma; }
	int getQ0() const { return this->q0; }
	set<int> getF() const { return this->F; }
	set<char> getGamma() const { return this->Gamma; }
	char getZ() const { return this->Z; }
	DeltaType getDelta() const { return this->delta; }

	bool isFinalStateAndIsEmptyStack(int);
	int deltaStar(int, string);
};

bool DPDA::isFinalStateAndIsEmptyStack(int q)
{
	return (F.find(q) != F.end()) && Stack.top() == this->Z;
}

int DPDA::deltaStar(int q, string w)
{
	if (Stack.size() == 0)
		return ErrorState;

	if (w.length() == 0)
	{
		pair<int, string> transition = delta[{ {q, Stack.top()}, Lambda}];
		Stack.pop();
		for (char symbol : transition.second)
			Stack.push(symbol);
		return transition.first;
	}
	if (w.length() == 1)
	{
		pair<int, string> transition = delta[{ {q, Stack.top()}, (char)w[0]}];
		Stack.pop();
		for (char symbol : transition.second)
			Stack.push(symbol);
		return transition.first;
	}
	pair<int, string> transition = delta[{ {q, Stack.top()}, (char)w[0]}];
	Stack.pop();
	for (char symbol : transition.second)
		Stack.push(symbol);

	if (delta[{ {transition.first, Stack.top()}, Lambda}].first != -1) {
		transition = delta[{ {transition.first, Stack.top()}, Lambda}];
		Stack.pop();
	}

	return deltaStar(transition.first, w.substr(1, w.length() - 1));
}

DPDA configureDPDA_3()
{
	set<int> Q = { 0, 1, 2, 3, 4, 5 };
	set<char> Sigma = { 'a', 'b' };
	int q0 = 0;
	set<int> F = { 5 };
	set<char> Gamma = { 'A', 'B' };
	char Z = 'Z';
	DeltaType delta;

	for (auto q : Q)
		for (auto s : Gamma)
			for (auto ch : Sigma)
			{
				delta[{ {q, s}, ch}] = { ErrorState, "" };
				delta[{ {q, s}, EmptyChar}] = { ErrorState, "" };
				delta[{ {q, s}, Lambda}] = { ErrorState, "" };
			}

	delta[{ {0, 'Z'}, 'a'}] = { 0, "ZA" };
	delta[{ {0, 'A'}, 'a'}] = { 0, "AA" };
	delta[{ {0, 'A'}, 'b'}] = { 1, "ABBB" };

	delta[{ {1, 'B'}, 'b'}] = { 1, "BBBB" };
	delta[{ {1, 'B'}, 'a'}] = { 2, "" };

	delta[{ {2, 'B'}, 'a'}] = { 2, "" };
	delta[{ {2, 'A'}, 'b'}] = { 3, "" };

	delta[{ {3, 'A'}, Lambda}] = { 4, "" };

	delta[{ {4, 'A'}, 'b'}] = { 3, "" };
	delta[{ {4, 'Z'}, Lambda}] = { 5, "Z" };

	DPDA M(Q, Sigma, q0, F, Gamma, Z, delta);
	return M;
}

int main()
{
	DPDA M = configureDPDA_3();
	//Pentru n=2 si m=1
	string word = "aaaabaaabb.";

	if (M.isFinalStateAndIsEmptyStack(M.deltaStar(M.getQ0(), word))) cout << "Cuvant acceptat";
	else cout << "Cuvant respins";

	return 0;
}