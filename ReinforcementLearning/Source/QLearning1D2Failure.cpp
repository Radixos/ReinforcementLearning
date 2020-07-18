#include "../Include/QLearning1D2Failure.h"

using namespace std;

QLearning1D2Failure::QLearning1D2Failure(int reinforcementTableCount, int startState, bool useQlambda)
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

#pragma region Creating "Q" array - action value function
	Q = new double* [reinforcementTableCount];	//action value function
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

	/* initialize random seed: */
	srand(time(NULL));

	while (steps <= max_steps)
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
		if (useQlambda)
			e_Q[currentState][action] += 1;	//accumulative mode (for replacing mode use just "=")
#pragma endregion Q(lambda) - learning

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

		path.push_back(action);

		int reward = reinforcementTable[nextState];

		double max_nextQ = Q[nextState][0];

		for (int i = 1; i < actionsCount; i++)
			if (Q[nextState][i] > max_nextQ)
				max_nextQ = Q[nextState][i];

		double delta;

		if (reward != -1)
			delta = reward + gamma * max_nextQ - Q[currentState][action];
		else
		{
			delta = reward - Q[currentState][action];
			nextState = startState;

			if (attempt == max_attempt)
				break;
		}

#pragma region Q(0) - learning
		if (!useQlambda)
			Q[currentState][action] += alfa * delta;	//Q(0) -learning
#pragma endregion Q(0) - learning

#pragma region Q(lambda) - learning
		if (useQlambda)
		{
			for (int s = 0; s < reinforcementTableCount; s++)	//s - state
				for (int a = 0; a < actionsCount; a++)	//a - action
					Q[s][a] += alfa * delta * e_Q[s][a];

			if (reward == -1)
				for (int s = 0; s < reinforcementTableCount; s++)	//s - state
					for (int a = 0; a < actionsCount; a++)	//a - action
						e_Q[s][a] = 0.f;
			else
				for (int s = 0; s < reinforcementTableCount; s++)	//s - state
					for (int a = 0; a < actionsCount; a++)	//a - action
						e_Q[s][a] *= gamma * lambda;
		}
#pragma endregion Q(lambda) - learning

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
	delete[] Q;
	delete[] e_Q;
}