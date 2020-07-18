#include "../Include/QLearning1D.h"

using namespace std;

QLearning1D::QLearning1D(int reinforcementTableCount, int rewardPlace, int startState, bool useQLambda)
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

#pragma region Creating "Q" array - action value function
	Q = new double* [reinforcementTableCount];	//action values function
	for (int i = 0; i < reinforcementTableCount; i++)
		Q[i] = new double[actionsCount];

	for (int i = 0; i < reinforcementTableCount; i++)
		for (int j = 0; j < actionsCount; j++)
			Q[i][j] = 0;
#pragma endregion Creating "Q" array - action value function

#pragma region Creating "e_Q" array - eligibility values function
	e_Q = new double* [reinforcementTableCount];	//eligibility values function (activity traces, pl: slady aktywnosci)
	for (int i = 0; i < reinforcementTableCount; i++)
		e_Q[i] = new double[actionsCount];

	for (int i = 0; i < reinforcementTableCount; i++)
		for (int j = 0; j < actionsCount; j++)
			e_Q[i][j] = 0;
#pragma endregion Creating "e_Q" array - eligibility values function

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

		double max_Q = Q[currentState][0];

		for (int i = 1; i < actionsCount; i++)
			if (Q[currentState][i] > max_Q)
				max_Q = Q[currentState][i];

		int j = 0;

		for (int i = 0; i < actionsCount; i++)
			if (Q[currentState][i] == max_Q)
				greedyActionsIndicies[j++] = i;

		double randNumber = (rand() % 100) / 100.f;

		if ((rand() % 100) / 100.f > epsilon)
			action = greedyActionsIndicies[(int)floor(randNumber * j)];
		else
			action = actions[(int)floor(randNumber * actionsCount)];

#pragma region Q(lambda) - learning
		if (useQLambda)
			e_Q[currentState][action] += 1;	//accumulative mode (for replacing mode use just "=")
#pragma endregion Q(lambda) - learning

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

		double max_nextQ = Q[nextState][0];

		for (int i = 1; i < actionsCount; i++)
			if (Q[nextState][i] > max_nextQ)
				max_nextQ = Q[nextState][i];

		double delta = reward + gamma * max_nextQ - Q[currentState][action];

#pragma region Q(0) - learning
		if (!useQLambda)
			Q[currentState][action] += alfa * delta;	//Q(0) -learning
#pragma endregion Q(0) - learning

#pragma region Q(lambda) - learning
		if (useQLambda)
		{
			for (int s = 0; s < reinforcementTableCount; s++)	//s - state
				for (int a = 0; a < actionsCount; a++)	//a - action
					Q[s][a] += alfa * delta * e_Q[s][a];

			if (reward == 1)
				for (int s = 0; s < reinforcementTableCount; s++)	//s - state
					for (int a = 0; a < actionsCount; a++)	//a - action
						e_Q[s][a] = 0.f;
			else
				for (int s = 0; s < reinforcementTableCount; s++)	//s - state
					for (int a = 0; a < actionsCount; a++)	//a - action
						e_Q[s][a] *= gamma * lambda;
		}
#pragma endregion Q(lambda) - learning

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
	delete[] Q;
	delete[] e_Q;
}