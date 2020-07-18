#include "../Include/AHC1D2Failure.h"

using namespace std;

AHC1D2Failure::AHC1D2Failure(int reinforcementTableCount, int startState, bool useAHCLambda)
{
	nextState = startState;

#pragma region Running conditions
	if (startState <= 0)
	{
		cout << "startState can not be equal to 0 or have a smaller value!" << endl;
		return;
	}

	if (startState >= reinforcementTableCount)
	{
		cout << "startState can not be equal to reinforcementTableCount or have a higher value!" << endl;
		return;
	}
#pragma endregion Running conditions

#pragma region Creating environment
	reinforcementTable = new int[reinforcementTableCount];	//map of environment
	for (int i = 1; i < reinforcementTableCount - 1; i++)
		reinforcementTable[i] = { 0 };

	reinforcementTable[0] = -1;
	reinforcementTable[reinforcementTableCount - 1] = -1;
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
		V[i] = { 1 };
#pragma endregion Creating "v" array - values function

#pragma region Creating "e_v" array - eligibility function for values
	e_V = new double[reinforcementTableCount];	//eligibility function for values
	for (int i = 0; i < reinforcementTableCount; i++)
		e_V[i] = { 0 };
#pragma endregion Creating "e_v" array - eligibility function for values

	/* initialize random seed: */
	srand(time(NULL));

	while (steps <= max_steps)
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

		double delta;

		if (reward != -1)
			delta = reward + gamma * V[nextState] - V[currentState];
		else
		{
			delta = reward - V[currentState];
			nextState = startState;

			if (attempt == max_attempt)
				break;
		}

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

			if (reward == -1)
				for (int s = 0; s < reinforcementTableCount; s++)	//s - state
					for (int a = 0; a < actionsCount; a++)	//a - action
						e_mi[s][a] = 0.f;
			else
				for (int s = 0; s < reinforcementTableCount; s++)	//s - state
					for (int a = 0; a < actionsCount; a++)	//a - action
						e_mi[s][a] *= gamma * mi_lambda;

			for (int s = 0; s < reinforcementTableCount; s++)	//s - state
				V[s] += v_alfa * delta * e_V[s];

			if (reward == -1)
				for (int s = 0; s < reinforcementTableCount; s++)	//s - state
					e_V[s] = 0.f;
			else
				for (int s = 0; s < reinforcementTableCount; s++)	//s - state
					e_V[s] *= gamma * v_lambda;
		}
#pragma endregion AHC(lambda)

		steps++;

		if (reward == -1 || steps == max_steps)
		{
			cout << "Attempt: " << attempt << endl;
			cout << "Steps: " << steps << endl;

			cout << "Path: " << endl;
			for (auto a : path)
				cout << a;

			cout << endl;
			attempt++;
			path.clear();

			if (steps == max_steps)
				break;
			steps = 0;
		}
	}

	delete[] reinforcementTable;
	delete[] mi;
	delete[] e_mi;
	delete[] V;
	delete[] e_V;
}