#include "../Include/Sarsa2D.h"
#include <iomanip>

using namespace std;

Sarsa2D::Sarsa2D(int startState, int maxRepetition, int maxEpisode, bool useSarsaLambda)
{
	performSimulation = true;

	nextState = startState;

#pragma region Running conditions
	state2rc(nextState, true);

	if (m_environment[m_new_r][m_new_c] != 0)
	{
		cout << "Agent placed on terminal state or on an obstacle." << endl;
		performSimulation = false;
	}

	if (nextState >= reinforcementTableCount || nextState < 0)
	{
		cout << "Agent placed outside the environment." << endl;
		performSimulation = false;
	}
#pragma endregion Running conditions

#pragma region Creating 2D steps array
	steps2D = new int* [maxEpisode];
	for (int i = 0; i < maxEpisode; i++)
		steps2D[i] = new int[maxRepetition];
#pragma endregion Creating 2D steps array

	/* initialize random seed: */
	srand(time(NULL));

	double max_Q;

	for (int repetition = 0; repetition < maxRepetition; repetition++)
	{
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

		for (int episode = 0; episode < maxEpisode; episode++)
		{
			nextState = startState;

			state2rc(nextState, true);

			while (performSimulation)
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

				nextState = getState(currentState, action);
				state2rc(nextState, true);

				double reward = m_environment[m_new_r][m_new_c];

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

#pragma region Sarsa(lambda)
				if (useSarsaLambda)
					e_Q[currentState][action] += 1;	//accumulative mode (for replacing mode use just "=")
#pragma endregion Sarsa(lambda)

				actionsPath.push_back(action);
				statesPath.push_back(nextState);

				double delta = reward + gamma * Q[nextState][nextAction] - Q[currentState][action];

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

					if (reward == 1)
						for (int s = 0; s < reinforcementTableCount; s++)	//s - state
							for (int a = 0; a < actionsCount; a++)	//a - action
								e_Q[s][a] = 0.f;
					else    //forgetting
						for (int s = 0; s < reinforcementTableCount; s++)	//s - state
							for (int a = 0; a < actionsCount; a++)	//a - action
								e_Q[s][a] *= gamma * lambda;
				}
#pragma endregion Sarsa(lambda)

				if (reward == 1)
				{
					cout << "Repetition: " << repetition + 1 << " ";
					cout << "Episode: " << episode + 1 << endl;

					/*cout << "Performed Actions: " << endl;
					for (auto a : actionsPath)
					{
						cout << a;
					}
					cout << endl;

					cout << "Visited States: " << endl;
					for (auto a : statesPath)
					{
						cout << a << ", ";
					}
					cout << endl;*/
					cout << endl;

					steps2D[episode][repetition] = statesPath.size();

					actionsPath.clear();
					statesPath.clear();
					nextState = startState;

					break;
				}

				action = nextAction;
			}
		}

		delete[] Q;
		delete[] e_Q;
	}

	DisplayInfo(maxEpisode, maxRepetition);

	delete[] steps2D;
}

void Sarsa2D::DisplayInfo(int maxEpisode, int maxRepetition)
{
	list <double> avgEpisode;
	double minEpisode;
	int minEpisodeInd;

	for (int episode = 0; episode < maxEpisode; episode++)
	{
		double tempEpisode = 0;

		for (int repetition = 0; repetition < maxRepetition; repetition++)
			tempEpisode += steps2D[episode][repetition];

		tempEpisode /= maxRepetition;

		if (episode == 0)
		{
			minEpisode = tempEpisode;
			minEpisodeInd = episode;
		}

		if (tempEpisode < minEpisode)
		{
			minEpisode = tempEpisode;
			minEpisodeInd = episode;
		}

		avgEpisode.push_back(tempEpisode);
	}

	cout << "Avg for all episodes: " << endl;
	for (auto a : avgEpisode)
		cout << setprecision(2) << fixed << a << ", ";

	cout << endl << endl;

	cout << "minEpisode: " << minEpisode << " at episode: " << minEpisodeInd << endl;
	//cout << "minEpisode: {0} at episode: {1}", minEpisode, minEpisodeInd;
}

int Sarsa2D::rc2state(int r, int c)
{
	m_state = r * m_environmentY + c;

	return m_state;
}

void Sarsa2D::state2rc(int state, bool ifNextState)
{
	if (ifNextState)
	{
		m_new_r = floor(state / m_environmentY);
		m_new_c = state % m_environmentY;
	}
	else
	{
		m_r = floor(state / m_environmentY);
		m_c = state % m_environmentY;
	}

}

int Sarsa2D::getState(int currentState, int action)
{
	state2rc(currentState, false);

	switch (action)
	{
	case 0:
		m_new_r = m_r;
		m_new_c = m_c - 1;
		break;
	case 1:
		m_new_r = m_r;
		m_new_c = m_c + 1;
		break;
	case 2:
		m_new_r = m_r - 1;
		m_new_c = m_c;
		break;
	case 3:
		m_new_r = m_r + 1;
		m_new_c = m_c;
		break;
	}

	if (!(m_new_r >= 0 && m_new_r < m_environmentX && m_new_c >= 0 &&
		m_new_c < m_environmentY) || m_environment[m_new_r][m_new_c] == -1)
	{
		m_new_r = m_r;
		m_new_c = m_c;
	}

	return m_nextState = rc2state(m_new_r, m_new_c);
}