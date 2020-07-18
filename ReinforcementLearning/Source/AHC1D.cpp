#include "../Include/AHC1D.h"

using namespace std;

AHC1D::AHC1D(int reinforcementTableCount, int rewardPlace, int startState, bool useAHCLambda)
{
	nextState = startState;

#pragma region Running conditions
	if (rewardPlace >= reinforcementTableCount)
		rewardPlace = reinforcementTableCount - 1;
	else if (rewardPlace < 0)
		rewardPlace = 0;

	if (rewardPlace == startState)
	{
		cout << "rewardPlace can not be equal to startState!" << endl;
		return;
	}
#pragma endregion Running conditions

#pragma region Creating environment
	reinforcementTable = new int[reinforcementTableCount];	//map of environment
	for (int i = 0; i < reinforcementTableCount; i++)
		reinforcementTable[i] = { 0 };

	reinforcementTable[rewardPlace] = 1;
#pragma endregion Creating environment

#pragma region Creating "mi" array - policy function
	mi = new double* [reinforcementTableCount];	//policy function
	for (int i = 0; i < reinforcementTableCount; i++)
		mi[i] = new double[actionsCount];

	for (int i = 0; i < reinforcementTableCount; i++)
		for (int j = 0; j < actionsCount; j++)
			mi[i][j] = 0;
#pragma endregion Creating "mi" array - policy function

#pragma region Creating "e_mi" array - eligibility values function for policy
	e_mi = new double* [reinforcementTableCount];	//eligibility values function for policy
	for (int i = 0; i < reinforcementTableCount; i++)
		e_mi[i] = new double[actionsCount];

	for (int i = 0; i < reinforcementTableCount; i++)
		for (int j = 0; j < actionsCount; j++)
			e_mi[i][j] = 0;
#pragma endregion Creating "e_mi" array - eligibility values function for policy

#pragma region Creating "v" array - values function
	V = new double[reinforcementTableCount];	//values function
	for (int i = 0; i < reinforcementTableCount; i++)
		V[i] = { 0 };
#pragma endregion Creating "v" array - values function

#pragma region Creating "e_v" array - eligibility function for values
	e_V = new double[reinforcementTableCount];	//eligibility function for values
	for (int i = 0; i < reinforcementTableCount; i++)
		e_V[i] = { 0 };
#pragma endregion Creating "e_v" array - eligibility function for values

#pragma region Creating optimalPath
	if (rewardPlace > startState)
		for (int i = 0; i < rewardPlace - startState; i++)
			optimalPath.push_back(1);
	else
		for (int i = 0; i < startState - rewardPlace; i++)
			optimalPath.push_back(0);
#pragma endregion Creating optimalPath

	/* initialize random seed: */
	srand(time(NULL));

	while (true)
	{
		currentState = nextState;

		double max_mi = mi[currentState][0];

		for (int i = 1; i < actionsCount; i++)
			if (mi[currentState][i] > max_mi)
				max_mi = mi[currentState][i];

		int j = 0;

		for (int i = 0; i < actionsCount; i++)
			if (mi[currentState][i] == max_mi)
				greedyActionsIndicies[j++] = i;

		double randNumber = (rand() % 100) / 100.f;

		if ((rand() % 100) / 100.f > epsilon)
			action = greedyActionsIndicies[(int)floor(randNumber * j)];
		else
			action = actions[(int)floor(randNumber * actionsCount)];

#pragma region AHC(lambda)
		if (useAHCLambda)
			e_mi[currentState][action] += 1;	//accumulative mode (for replacing mode use just "=")
#pragma endregion AHC(lambda)

#pragma region Determining nextState
		switch (action)
		{
		case 0:
			nextState = currentState - 1;
			if (nextState < 0)
				nextState = currentState;
			break;
		case 1:
			nextState = currentState + 1;
			if (nextState > reinforcementTableCount - 1)
				nextState = currentState;
			break;
		}
#pragma endregion Determining nextState

		path.push_back(action);

		int reward = reinforcementTable[nextState];

		double delta = reward + gamma * V[nextState] - V[currentState];

#pragma region AHC(0)
		if (!useAHCLambda)
		{
			mi[currentState][action] += mi_alfa * delta;	//AHC(0)
			V[currentState] += v_alfa * delta;
		}
#pragma endregion AHC(0)

#pragma region AHC(lambda)
		if (useAHCLambda)
		{
			for (int s = 0; s < reinforcementTableCount; s++)	//s - state
				for (int a = 0; a < actionsCount; a++)	//a - action
					mi[s][a] += mi_alfa * delta * e_mi[s][a];

			if (reward == 1)
				for (int s = 0; s < reinforcementTableCount; s++)	//s - state
					for (int a = 0; a < actionsCount; a++)	//a - action
						e_mi[s][a] = 0.f;
			else
				for (int s = 0; s < reinforcementTableCount; s++)	//s - state
					for (int a = 0; a < actionsCount; a++)	//a - action
						e_mi[s][a] *= gamma * mi_lambda;
		
			for (int s = 0; s < reinforcementTableCount; s++)	//s - state
				V[s] += v_alfa * delta * e_V[s];

			if (reward == 1)
				for (int s = 0; s < reinforcementTableCount; s++)	//s - state
					e_V[s] = 0.f;
			else
				for (int s = 0; s < reinforcementTableCount; s++)	//s - state
					e_V[s] *= gamma * v_lambda;
		}
#pragma endregion AHC(lambda)

		if (nextState == rewardPlace)
		{
			cout << "Attempt: " << attempt << endl;

			cout << "Path: " << endl;
			for (auto a : path)
				cout << a;

			cout << endl;

			if (path.size() == optimalPath.size())
				if (path == optimalPath)
					break;

			path.clear();
			attempt++;
			nextState = startState;
		}
	}

	delete[] reinforcementTable;
	delete[] mi;
	delete[] e_mi;
	delete[] V;
	delete[] e_V;
}