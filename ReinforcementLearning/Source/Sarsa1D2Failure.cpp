#include "../Include/Sarsa1D2Failure.h"

using namespace std;

Sarsa1D2Failure::Sarsa1D2Failure(int reinforcementTableCount, int startState, bool useSarsaLambda)
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

	double max_Q;
	
	while (steps <= max_steps)
	{
		currentState = nextState;

#pragma region First run for t0
		if (t0 == true)
		{
			t0 = false;

			max_Q = Q[currentState][0];

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
		}
#pragma endregion First run for t0

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

		int reward = reinforcementTable[nextState];
		
		path.push_back(action);

#pragma region Sarsa(lambda)
		if (useSarsaLambda)
			e_Q[currentState][action] += 1;	//accumulative mode (for replacing mode use just "=")
#pragma endregion Sarsa(lambda)

		max_Q = Q[nextState][0];

		for (int i = 1; i < actionsCount; i++)
			if (Q[nextState][i] > max_Q)
				max_Q = Q[nextState][i];

		int j = 0;

		for (int i = 0; i < actionsCount; i++)
			if (Q[nextState][i] == max_Q)
				greedyActionsIndicies[j++] = i;

		double randNumber = (rand() % 100) / 100.f;

		if ((rand() % 100) / 100.f > epsilon)
			nextAction = greedyActionsIndicies[(int)floor(randNumber * j)];
		else
			nextAction = actions[(int)floor(randNumber * actionsCount)];

		double delta;

		if (reward != -1)
			delta = reward + gamma * Q[nextState][nextAction] - Q[currentState][action];
		else
		{
			delta = reward - Q[currentState][action];
			nextState = startState;

			if (attempt == max_attempt)
				break;
		}

#pragma region Sarsa(0)
		if (!useSarsaLambda)
			Q[currentState][action] += alfa * delta;	//Sarsa(0)
#pragma endregion Sarsa(0)

#pragma region Sarsa(lambda)
		if (useSarsaLambda)
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
#pragma endregion Sarsa(lambda)

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

		action = nextAction;
	}

	delete[] reinforcementTable;
	delete[] Q;
	delete[] e_Q;
}